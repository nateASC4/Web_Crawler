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

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
using namespace std;

void makeThread(){
   pthread_t threads[NUM_THREADS];
   int rc;
   int i;
   for( i = 0; i < NUM_THREADS; i++ ) {
      cout << "main() : creating thread, " << i << endl;
      rc = pthread_create(&threads[i], NULL, PrintHello, (void *)i);
      if (rc) {
         printf("Error:unable to create thread, %d
", rc);
         exit(-1);
      }
   }
   pthread_exit(NULL);
}

void read_input(){
   /* basic read file function plus loop*/
   //use fgets() for security
   FILE* ptr; 
   char count;

   ptr = fopen("url.txt", r);

   char buffer[//size of file];
   fgets(buffer, size_of_file, stdin);
}

void readURL(){
   //if URL has a .com,.net,.gov,.io, then RUN
   //else THROW ERROR exception
}

void extractInfo(){
   //what information will we be extracting 
   // wiil need to ask him
   // perhaps we should use a map to store the data
   // most likely a stack if a map is to hard to implement 
}

#define NUM_THREADS 5
#define size_of_file 1024 // will define later
void *PrintHello(void *threadid) {
   long tid;
   tid = (long)threadid;
   printf("Hello World! Thread ID, %d
", tid);
   pthread_exit(NULL);
}

// this function reads a file as input which contains multiples links and urls

int main () {
   //call our functions
}