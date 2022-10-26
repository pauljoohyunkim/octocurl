#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <curl/curl.h>

#include "octoget.h"
#include "conn.h"

unsigned int concurrentDownloadNum = DEFAULT_CONCURRENT_DOWNLOAD;
unsigned int numOfURLs = 0;
unsigned int queueNum = 0;
char** URLs;                    // Array of URLs
bool qURLsAllocated = false;    // Whether or not the URL array is allocated or not.
pthread_t* threadPtr;           // Array of Threads
bool qThreadAllocated = false;  // Whether or not the threadPtr is allocated or not.
Status** statuses;              // Array of Status struct pointers for each worker.
bool qStatusAllocated = false;  // Status structs allocated
bool qCurlGlobalInitialized = false;   // Curl Global Initialization
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char* argv[])
{
    signal(SIGINT, handler);
    // The threads for downloading individual files.
    //pthread_t* workers;
    //
    
    if(argc==1)
    {
        showHelp();
    }
    
    
    int c;              // Option
    while((c = getopt(argc, argv, "c:")) != -1)
    {
        switch(c)
        {
            case 'c':
                char *ptr;
                concurrentDownloadNum = strtol(optarg, &ptr, 10);
                
                // Check if the number of workers is valid.
                // if(concurrentDownloadNum >= MIN_CONCURRENT_DOWNLOAD && concurrentDownloadNum <= MAX_CONCURRENT_DOWNLOAD && ptr == optarg) (MIGHT NEED TO CHECK FOR STRTOL RETURN SUCCESS)
                if(concurrentDownloadNum >= MIN_CONCURRENT_DOWNLOAD && concurrentDownloadNum <= MAX_CONCURRENT_DOWNLOAD)
                {
                    printf("The number of concurrent download workers set to %u.\n", concurrentDownloadNum);
                }
                else
                {
                    fprintf(stderr, "The number of concurrent download workers should be between %d and %d\n", MIN_CONCURRENT_DOWNLOAD, MAX_CONCURRENT_DOWNLOAD);
                    return 1;
                }
                break;
        }
    }

    // CURL Global Setup
    qCurlGlobalInitialized = true;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Iterate to count the numbers of URLs parsed through command line
    for(; optind < argc; optind++)
    {
        numOfURLs++;
    }
    optind = 1;         // Reset optind to iterate through the extra arguments again.
    qURLsAllocated = true;
    URLs = (char**) malloc(numOfURLs * sizeof(char*));  // URLs is now an array to hold urls.
    unsigned int URLIndex = 0;
    for(; optind < argc; optind++)
    {
        URLs[URLIndex] = argv[optind];      // Copying pointer to each url to URLs array.
        printf("Added to queue: %s\n", URLs[URLIndex]);
        URLIndex++;
    }

    // Status Struct Memory Allocation for each worker
    qStatusAllocated = true;
    statuses = (Status**) malloc(concurrentDownloadNum * sizeof(Status*));
    // Allocating each struct
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        statuses[index] = (Status*) malloc(sizeof(Status*));
    }

    // Allocate pointer to threads
    qThreadAllocated = true;
    threadPtr = (pthread_t*) malloc(concurrentDownloadNum * sizeof(pthread_t));
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        pthread_create(&threadPtr[index], NULL, queueWorker, statuses[index]);
    }

    





    // Wait until all threads are closed.
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        pthread_join(threadPtr[index], NULL);
    }

    printf("Queue finished.\n");


    // Garbage Collection
    free(URLs);
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        free(statuses[index]);
    }
    free(statuses);
    free(threadPtr);

    // Curl Exit
    curl_global_cleanup();

    return 0;
}

// Handles Interrupt Signal
void handler(int num)
{
    // extern bool qURLsAllocated;
    switch(num)
    {
        case SIGINT:
            write(STDERR_FILENO, "SIGINT received: Quitting\n", 26);
            if(qURLsAllocated == true)
            {
                free(URLs);
            }
            if(qStatusAllocated == true)
            {
                for(unsigned int index = 0; index < concurrentDownloadNum; index++)
                {
                    free(statuses[index]);
                }
                free(statuses);
            }
            if(qThreadAllocated == true)
            {
                free(threadPtr);
            }
            exit(1);
            break;
    }
}

void showHelp()
{
    printf("Usage: octoget [options] url1 [url2] [url...]\n"
           "\n"
           "-c x\tSpecify the number of concurrent downloads (Default: 4)\n"
          );
    exit(1);
}
