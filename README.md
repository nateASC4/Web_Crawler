# Web_Crawler
This project will detail a basic webcrawler

/*
On the day of submission, you are also required to submit the GitHub link for 
your entire project. 
A web crawler is a program that automatically navigates the web pages and extracts useful 
information from them. The goal of this project is to develop a multithreaded web crawler that 
can efficiently crawl and extract information from multiple web pages simultaneously.
1. Develop a multithreaded web crawler in C.
2. crawl multiple web pages concurrently.
      Define a list of URLs to scrape.
     Use a loop to iterate through the list of URLs.
      For each URL, send a request to the website and extract the data you need using your web 
     scraping tool of choice.
       Store the scraped data in a structured format, such as a CSV or JSON file.
     Repeat the process for each URL in the list.
3. The crawler should extract and store relevant information such as any links present on 
the page.
  1) URLs
  2) Title
  3) Meta Description
  4) Links
  5) Text Content
  
4. follow links on the page to other pages and continue the crawling process.
5. handle errors and exceptions, such as invalid URLs or 
unavailable pages.
6. The extracted information should be stored in an appropriate data structure, such as a 
database or a file.
7. The input must be in the form of a text file containing multiple links/URLs which will be 
parsed as input to the program.
*/


//crawl websites on each thread???
// create a shared, thread-safe list with the URL's to be crawled. create an Executor with the number of threads corresponding to the number of crawlers you desire to run concurrently. start your crawlers as Runnables with a reference to the shared list and submit each of them to the Executor. each crawler removes the next URL from the list and does whatever you need it to do, looping until the list is empty.

Use the curl library 
https://www.webscrapingapi.com/c-web-scraping
we may need to use several files 

Develop a multithreaded web crawler in C: This involves writing code in C to create a web crawler that can navigate the internet and extract information from web pages. The crawler should be multithreaded to allow for crawling multiple web pages concurrently.

The crawler should be able to crawl multiple web pages concurrently: To achieve this, you can use a thread pool to manage multiple threads that crawl web pages. The thread pool can maintain a queue of URLs to crawl and allocate threads to process the URLs.

The crawler should extract and store relevant information such as any links present on the page: To extract information from web pages, you can use a library like Gumbo to parse the HTML pages and extract links and other relevant information.

The crawler should be able to follow links on the page to other pages and continue the crawling process: To follow links on web pages, you can use a queue to maintain a list of URLs to crawl. Each time a thread processes a web page, it can extract links and add them to the queue.

The crawler should be able to handle errors and exceptions, such as invalid URLs or unavailable pages: To handle errors, you can use error handling mechanisms in C to detect errors and take appropriate action. For example, if a web page is unavailable, you can skip it and move on to the next URL in the queue.

The extracted information should be stored in an appropriate data structure, such as a database or a file: To store the extracted information, you can use a database or a file to store the data. You can use a library like SQLite to store the data in a database.

The input must be in the form of a text file containing multiple links/URLs which will be parsed as input to the program: You can write code to read URLs from a text file and add them to the URL queue.

In addition to the above requirements, you should also consider the following:

The web crawler should be designed to be efficient and not overload websites with requests.
You should follow best practices for web crawling, including following robots.txt files and respecting website terms of service.
You should test the web crawler thoroughly and refine it as needed to ensure it meets the requirements and works correctly.