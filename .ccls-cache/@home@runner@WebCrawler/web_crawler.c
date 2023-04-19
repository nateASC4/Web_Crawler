
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
struct Crawled_Data{
  char author[100];
  char links[100];
  char metadata[100];
  char cookies[100];
};


//create 10 threads (1 thread per website) for each website
void create_threads(){
  //here we will create each thread using the POSIX library 
  pthread_t url_threads[10]; // 10 urls
  int rc = 0; //return valu
  
  for(int i = 0; i < 10; i++){
    rc = pthread_create(&url_threads[i], NULL, /*libcurl function*/, (void *)&websites[i]);
    if (rc) {
      printf("Error: failed to create thread for website %s\n", websites[i].url);
      exit(-1);
    }
  }

  // Wait for all threads to finish
  for (int i = 0; i < 10; i++) {
    rc = pthread_join(&url_threads[i], NULL);
    if (rc) {
      printf("Error: failed to join thread %d\n", i);
      exit(-1);
    }
  }

  // Write the data to a file or a database
  // ...
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
      
  }
}
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
      
  }

//establish a connection to each website
void step3_create_connection(){
  
}
//this method will send request to website 
void step2_send_request(){
  //now that we read the file, we know which website to crawl first
  step1_scanFile();
  //so as we crawl we add the information to an array
  

  
}
///perhaps make an array a global variable
//this method stores crawled information in an array or maybe 2D array 
void crawled_info(){
  
}

void writeToFile(){
  //this will be the last function as we will write to the file after we get 
  FILE *fp = fopen("data.txt","w");
   if (fp == NULL) {
        printf("Error opening file!\n");
    }else{
     //Write crawled information to file 
    char arr[20][100]; 
    for (int i =0; i < sizeof(arr); i++){
      
    }
    //do you want to check for sanitzation of the URL 
//loop throug the URLs 

//First Step to make a basic web crawler 
int currentIndex = 0;

}


