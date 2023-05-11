/*This code defines a simple web crawler that fetches web pages using the libcurl library and parses them using the libxml library.*/
// Maximum number of connections
int max_con = 200;
// Total pages to crawl
int max_total = 200;
// Maximum number of requests/page
int max_requests = 50;
int max_link_per_page = 5;
int follow_relative_links = 0;

// including the necessary libraries
// used to parse HTML pages and extract links from them
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/uri.h> 
// used to detect and crawl links
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
// used for threading
#include <pthread.h>

pthread_mutex_t count_mutex;

// Set a cap on URL length to minimize impacts to performance
#define MAX_URL_LENGTH 1500


/*define a signal handler that allows the user to interrupt the program using CTRL-C*/
int pending_interrupt = 0;
void sighandler(int dummy)
{
  pending_interrupt = 1;
}

/* resizable buffer */
typedef struct {
  char *buf;
  size_t size;
} memory;

/*callback function that is used to handle the incoming data from an HTTP request. It stores the response data in a dynamically growing memory buffer.*/
size_t grow_buffer(void *contents, size_t sz, size_t nmemb, void *ctx)
{
  size_t realsize = sz * nmemb;
  memory *mem = (memory*) ctx;
  char *ptr = realloc(mem->buf, mem->size + realsize);
  if(!ptr) {
    /* out of memory */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  mem->buf = ptr;
  memcpy(&(mem->buf[mem->size]), contents, realsize);
  mem->size += realsize;
  return realsize;
}

/*The make_handle function creates a CURL handle and sets various options for it, including the URL to fetch and the callback function to use to 
handle the response data.*/
CURL *make_handle(char *url)
{
  CURL *handle = curl_easy_init();

  /* Important: use HTTP2 over HTTPS */
  curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(handle, CURLOPT_URL, url);

  /* buffer body */
  memory *mem = malloc(sizeof(memory));
  mem->size = 0;
  mem->buf = malloc(1);
  //write the response data from an HTTP request to a dynamically growing memory buffer using a callback function called grow_buffer.
  // function that will be used to handle the incoming data
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, grow_buffer);
  // pointer to the buffer that will store data
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, mem);
  // private pointer associated with this handle to memory
  curl_easy_setopt(handle, CURLOPT_PRIVATE, mem);

  /* Error handling */
  curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
  curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 10L);
  curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, 2L);
  curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "");
  curl_easy_setopt(handle, CURLOPT_FILETIME, 1L);
  curl_easy_setopt(handle, CURLOPT_USERAGENT, "mini crawler");
  curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
  curl_easy_setopt(handle, CURLOPT_UNRESTRICTED_AUTH, 1L);
  curl_easy_setopt(handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
  curl_easy_setopt(handle, CURLOPT_EXPECT_100_TIMEOUT_MS, 0L);



  return handle;
}

// This function takes a CURL multi-handle, a memory buffer containing HTML content, and a URL as input.
// It parses the HTML to extract all href attributes from <a> tags and adds the corresponding URLs to the multi-handle.
// It returns the number of URLs added to the multi-handle.
size_t follow_links(CURLM *multi_handle, memory *mem, char *url)
{
  // Set HTML parsing options to exclude blank nodes and suppress error and warning messages.
  int opts = HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | \
             HTML_PARSE_NOWARNING | HTML_PARSE_NONET;
  // Parse the HTML content from the memory buffer.
  htmlDocPtr doc = htmlReadMemory(mem->buf, mem->size, url, NULL, opts);
  // If parsing fails, return 0.
  if(!doc)
    return 0;
  // Define an XPath expression to select all href attributes from <a> tags.
  xmlChar *xpath = (xmlChar*) "//a/@href";
   // Create an XPath context for the parsed HTML document.
  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  // Evaluate the XPath expression on the HTML document
  xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
  xmlXPathFreeContext(context);
  if(!result)
    return 0;
  // Extract the set of nodes matching the XPath expression.
  xmlNodeSetPtr nodeset = result->nodesetval;
  // If the nodeset is empty, free the XPath object and return 0.
  if(xmlXPathNodeSetIsEmpty(nodeset)) {
    xmlXPathFreeObject(result);
    return 0;
  }
  // Initialize a counter for the number of URLs added to the multi-handle.
  size_t count = 0;
  int i;
  // Iterate over the set of nodes matching the XPath expression.
  for(i = 0; i < nodeset->nodeNr; i++) {
    double r = rand();
    int x = r * nodeset->nodeNr / RAND_MAX;
    // Extract the href attribute value from the selected node.
    const xmlNode *node = nodeset->nodeTab[x]->xmlChildrenNode;
    xmlChar *href = xmlNodeListGetString(doc, node, 1);
    // If follow_relative_links is enabled, resolve relative URLs against the current URL.
    if(follow_relative_links) {
      xmlChar *orig = href;
      href = xmlBuildURI(href, (xmlChar *) url);
      xmlFree(orig);
    }
    // Convert the href value to a C string.
    char *link = (char *) href;
    if(!link || strlen(link) < 20)
      continue;
    // If the link starts with "http://" or "https://", add it to the multi-handle.
    if(!strncmp(link, "http://", 7) || !strncmp(link, "https://", 8)) {
      curl_multi_add_handle(multi_handle, make_handle(link));
      if(count++ == max_link_per_page)
        break;
    }
    xmlFree(link);
  }
  // Free the XPath object and return the URL count.
  xmlXPathFreeObject(result);
  return count;
}

// function to check if a content type is HTML
int is_html(char *ctype)
{
  return ctype != NULL && strlen(ctype) > 10 && strstr(ctype, "text/html");
}

// function to crawl a webpage and follow links
void *crawl(void* ptr) {

  pthread_mutex_lock(&count_mutex);

  // cast void pointer argument to a char pointer
  char* start_page;
  start_page = (char* )ptr;

  // print the starting page
  printf("%s\n", start_page);
  // set up a signal handler for interrupting the program with Ctrl+C
  signal(SIGINT, sighandler);

  // Initialize LibXML library
  LIBXML_TEST_VERSION;

  // Initialize the libcurl library
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // create a multi-handle to perform multiple HTTP requests in parallel using libcurl
  CURLM *multi_handle = curl_multi_init();

  // set the maximum number of total connections and maximum number of connections per host
  curl_multi_setopt(multi_handle, CURLMOPT_MAX_TOTAL_CONNECTIONS, max_con);
  curl_multi_setopt(multi_handle, CURLMOPT_MAX_HOST_CONNECTIONS, 6L);

  /* enables http/2 if available */
#ifdef CURLPIPE_MULTIPLEX
  curl_multi_setopt(multi_handle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
#endif
    /* sets html start page */
  curl_multi_add_handle(multi_handle, make_handle(start_page));

  int msgs_left;
  int pending = 0;
  int complete = 0;
  int still_running = 1;

  // loop until all requests have completed or the program is interrupted
  while(still_running && !pending_interrupt) {
    int numfds;

    // wait for activity on any of the sockets being used by libcurl
    curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds);

    // perform any pending transfers on the multi-handle
    curl_multi_perform(multi_handle, &still_running);

    /* See how the transfers went */
    // read messages from the multi-handle
    CURLMsg *m = NULL;
    while((m = curl_multi_info_read(multi_handle, &msgs_left))) {
      if(m->msg == CURLMSG_DONE) {
        CURL *handle = m->easy_handle;
        char *url;
        memory *mem;

        // retrieve the memory buffer associated with the transfer
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &mem);

        // retrieve the URL of the transfer
        curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &url);

        if(m->data.result == CURLE_OK) {
          long res_status;
          curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &res_status);

          if(res_status == 200) {
            char *ctype;
            // retrieve the content type of the response          
            curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &ctype);


            // print the HTTP status code and URL to the console and output file
            FILE* fptr = fopen("output.txt", "a");
            fprintf(fptr, "[%d] HTTP %d: %s\n", complete, (int) res_status, url);
            fclose(fptr);
            printf("[%d] HTTP 200 (%s): %s\n", complete, ctype, url);
            
            
            // if the content type is HTML and there is enough memory:
            if(is_html(ctype) && mem->size > 100) {
              if(pending < max_requests && (complete + pending) < max_total) {
                pending += follow_links(multi_handle, mem, url);
                still_running = 1;
              }
            }
          }
          else {
            printf("[%d] HTTP %d: %s\n", complete, (int) res_status, url);

          }
        }
        else {
          printf("[%d] Connection failure: %s\n", complete, url);
        }
        curl_multi_remove_handle(multi_handle, handle);
        curl_easy_cleanup(handle);
        free(mem->buf);
        free(mem);
        complete++;
        pending--;
      }

    }
	  pthread_mutex_unlock(&count_mutex);

  }
  // Clean the multi-handle that was initialized
  curl_multi_cleanup(multi_handle);
  curl_global_cleanup();
}

int main(int argc, char *argv[])
{
  // taking file as inoput and making an array of urls
  int numURLs = 20;
  char urls[numURLs][100];
  pthread_t *threads[5];

  FILE* file = fopen(argv[1], "r");
  if (file == NULL) {
        printf("\n\nERROR: Check file input\n\n");
        exit(1);
    }



    int i = 0;
    while (fgets(urls[i], MAX_URL_LENGTH, file) != NULL) {
        urls[i][strcspn(urls[i], "\n")] = '\0';
        i++;
        if (i >= 100) {
            break;
        }
    }

    fclose(file);

    // multithreading part
    for (int j = 0; j < i; j++) {
        printf("%s\n", urls[j]);
    }
  
    for (int j = 0; j < i; j++) {
        pthread_create(&threads[j], NULL, crawl, (void *)urls[j]);
    }
    for (int j = 0; j < i; j++) {
        pthread_join(&threads[j], NULL);
    }
    pthread_exit(NULL);

  return 0;
}