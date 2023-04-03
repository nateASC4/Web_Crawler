/*
This a group project in which you are required to form a group of 3 and submit the project on the 
day of final submission by presenting a live demo of your own WebCrawler along with properly 
formatted code files. On the day of submission, you are also required to submit the GitHub link for 
your entire project. 
A web crawler is a program that automatically navigates the web pages and extracts useful 
information from them. The goal of this project is to develop a multithreaded web crawler that 
can efficiently crawl and extract information from multiple web pages simultaneously.
1. Develop a multithreaded web crawler in C.
2. The crawler should be able to crawl multiple web pages concurrently.
3. The crawler should extract and store relevant information such as any links present on 
the page.
4. The crawler should be able to follow links on the page to other pages and continue the 
crawling process.
5. The crawler should be able to handle errors and exceptions, such as invalid URLs or 
unavailable pages.
6. The extracted information should be stored in an appropriate data structure, such as a 
database or a file.
7. The input must be in the form of a text file containing multiple links/URLs which will be 
parsed as input to the program.
*/


//crawl websites on each thread???

#include <pthread.h> ///for POSIX multithreading 
#include <cstidlib>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>
#define NUM_THREADS 6
#include "crawler.h"
#include "io.c"

#define MAX_LINKS 10
#define MAX_URL_LEN 512

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
