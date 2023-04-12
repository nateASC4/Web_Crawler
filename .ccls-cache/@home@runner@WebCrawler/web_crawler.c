
#include <stdio.h> // scanning through files 
#include <string.h> // scanning through files 
#include <curl/curl.h> // to send HTTP requests not available in Replit 
#include <libxml/HTMLparser.h> // for parsing HTML and XML 
#include <pthread.h> ///for POSIX multithreading 
#include <stdlib.h> // scanning through files 

#define NUM_THREADS 6
#define MAX_LINKS 10
//max URL length
#define MAX_URL_LENGTH 2083 
#define buffer [MAX_URL_LENGTH]


//In this program we will follow SOLID principles to ensure clarity and improve functionality of our code

//scan a file for URL
      // Check if filename is provided as an argument
int step1_scanFile(){
  int scan = 0; 
    if (scan != 2) {
        printf("Usage: ./scan_urls <filename>\n");
        return 1;
    }

    // Open file
    FILE* fp = fopen("url.txt", "r");
    if (fp == NULL) {
        printf("Error: could not open file %s\n", "url.txt");
        return 1;
    }
  //we need to iterate through the list in a loop

    char url[MAX_URL_LENGTH];
    while (fgets(url, MAX_URL_LENGTH, fp) != NULL) {
        // Remove trailing newline character if present
        if (url[strlen(url) - 1] == '\n') {
            url[strlen(url) - 1] = '\0';
        }
      
        // Check if url starts with http:// or https://
        if (strncmp(url, "http://", 7) == 0 || strncmp(url, "https://", 8) == 0) { 
          //use strncmp to avoid buffer overflows
            printf("Found URL: %s\n", url);
        }
        //if url does not exist 
      else {
        printf("%s","404:URL not FOUND");
      }
    }
    // Close file
    fclose(fp);

    return 0;
}
//initialize the libcurl library 
void step0_initlibcurl(){
  CURL* curler;
  CURLcode res;

  curler  = curl_easy_init();
  res = curl_easy_perform(curler); 

      // Check if the curl request was successful
  if (res != CURLE_OK) {
      printf("Error performing curl request: %s\n", curl_easy_strerror(res));
      return 1;

  
}
//this method will send request to website 
void step2_send_request(){
  
}

int writeToFile(){
  //this will be the last function as we will write to the file after we get 
}
  //do you want to check for sanitzation of the URL 
//loop throug the URLs 

//First Step to make a basic web crawler 
int currentIndex = 0;
























size_t writeCallback(char * buffer, size_t size, size_t nmemb, void * userp) {
  TidyBuffer * tidyBuffer = (TidyBuffer *) userp;
  size_t newSize = size * nmemb;
  tidyBufAppend(tidyBuffer, buffer, newSize);
  return newSize;
};

void traverseTree(TidyNode node, char ** output) {
  TidyNode child;
  for (child = tidyGetChild(node); child != NULL; child = tidyGetNext(child)) {
    TidyAttr hrefAttr = tidyAttrGetById(child, TidyAttr_HREF);
    if (hrefAttr) {
      if (currentIndex < MAX_LINKS) {
        if (strlen(tidyAttrValue(hrefAttr)) < MAX_URL_LEN) {
          strcpy(output[currentIndex], tidyAttrValue(hrefAttr));
          currentIndex++;
        }
      }
      if (tidyAttrValue(hrefAttr)) {
        printf("Url found: %s\n", tidyAttrValue(hrefAttr));
      }
    }
    traverseTree(child, output);
  }
}

int crawlWebsite(Crawler crawler) {
  if (crawler.url) {
    CURL *curlHandle = curl_easy_init();
    char errorBuffer[CURL_ERROR_SIZE];
    int responseCode;
    TidyDoc tidyDoc;
    TidyBuffer tidyBuffer = {0};
    TidyBuffer tidyErrorBuffer = {0};
    char ** parsedUrls = (char **) malloc(MAX_LINKS * sizeof(char *));
    for (int i = 0; i < MAX_LINKS; i++) {
      parsedUrls[i] = (char *) malloc(MAX_URL_LEN * sizeof(char));
    }
    if (curlHandle) {
      curl_easy_setopt(curlHandle, CURLOPT_URL, crawler.url);
      curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
      curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &tidyBuffer);
      curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errorBuffer);
      tidyDoc = tidyCreate();
      tidyBufInit(&tidyBuffer);
      tidyOptSetInt(tidyDoc, TidyWrapLen, 2048);
      tidyOptSetBool(tidyDoc, TidyForceOutput, yes);
      responseCode = curl_easy_perform(curlHandle);
      if (responseCode == CURLE_OK) {
        printf("Successful crawl of %s\n", crawler.url);
        tidyParseBuffer(tidyDoc, &tidyBuffer);
        traverseTree(tidyGetBody(tidyDoc), parsedUrls);
        for (int i = 0; i < MAX_LINKS; i++) {
          if (parsedUrls[i]) {
            printf("writing %d: %s\n", i, parsedUrls[i]);
            openFileAndWrite(OUTPUT_PATH, parsedUrls[i]);
          }
        }
      } else {
        printf("Crawl failed for %s\n", crawler.url);
        return 0;
      }
      curl_easy_cleanup(curlHandle);
      tidyBufFree(&tidyBuffer);
      tidyBufFree(&tidyErrorBuffer);
      tidyRelease(tidyDoc);
      return 1;
    }
    return 0;
  }
  return 0;
}
