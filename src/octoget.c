#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <curl/curl.h>
#include <ncurses.h>

#include "octoget.h"
#include "conn.h"
#include "queue.h"
#include "structs.h"

unsigned int concurrentDownloadNum = DEFAULT_CONCURRENT_DOWNLOAD;
unsigned int numOfURLs = 0;
unsigned int queueNum = 0;
URLQueue** queues;              // This replaced the array of URLs used before.
bool qQueuesAllocated = false;
//char** URLs;                    // Array of URLs
//bool qURLsAllocated = false;    // Whether or not the URL array is allocated or not.
pthread_t* threadPtr;           // Array of Threads
bool qThreadAllocated = false;  // Whether or not the threadPtr is allocated or not.
Status** statuses;              // Array of Status struct pointers for each worker.
bool qStatusAllocated = false;  // Status structs allocated
bool qCurlGlobalInitialized = false;   // Curl Global Initialization
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;      // Mutex Lock for Worker Queue Read
unsigned int* URLArgIndices;    // Indicies from argv which correspond to URL
bool qURLArgIndicesAllocated = false;
int terminalWidth;                 // The columns and rows for ncurses

bool optS = false;              // Whether or not quicksort is to be used or not.
bool optP = false;              // Whether or not to prefetch file size. (false for prefetching, true for not prefetching)
int main(int argc, char* argv[])
{
    // Array of indices for URL arguments.
    int c;              // Command line short options

    signal(SIGINT, handler);

    // ncurses window start
    initscr();
    // Allocate index array for URL arguments.
    qURLArgIndicesAllocated = true;
    URLArgIndices = (unsigned int*) malloc(argc * sizeof(unsigned int));
    // CURL Global Setup
    qCurlGlobalInitialized = true;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // No argument specified -> Help Menu
    if(argc==1)
    {
        showHelp();
    }
    
    // ARGUMENT PARSING START
    while((c = getopt(argc, argv, "hc:sp")) != -1)
    {
        switch(c)
        {
            case 'h':
                showHelp();
            case 'c':
                char *ptr;
                concurrentDownloadNum = strtol(optarg, &ptr, 10);
                
                // Check if the number of workers is valid.
                // if(concurrentDownloadNum >= MIN_CONCURRENT_DOWNLOAD && concurrentDownloadNum <= MAX_CONCURRENT_DOWNLOAD && ptr == optarg) (MIGHT NEED TO CHECK FOR STRTOL RETURN SUCCESS)
                if(concurrentDownloadNum >= MIN_CONCURRENT_DOWNLOAD && concurrentDownloadNum <= MAX_CONCURRENT_DOWNLOAD)
                {
                    printw("The number of concurrent download workers set to %u.\n", concurrentDownloadNum);
                    refresh();
                }
                else
                {
                    fprintf(stderr, "The number of concurrent download workers should be between %d and %d\n", MIN_CONCURRENT_DOWNLOAD, MAX_CONCURRENT_DOWNLOAD);
                    return 1;
                }
                break;
            case 's':
                optS = true;
                printw("Quicksort will be used to download larger files first.\n");
                break;
            case 'p':
                optP = true;
                printw("File size prefetch disabled\n");
                break;
        }
    }


    // Iterate to count the numbers of URLs parsed through command line
    for(; optind < argc; optind++)
    {
        URLArgIndices[numOfURLs] = optind;
        numOfURLs++;
    }

    // Queue of URLs
    qQueuesAllocated = true;
    queues = (URLQueue**) malloc(numOfURLs * sizeof(URLQueue*));
    for(int index = 0; index < numOfURLs; index++)
    {
        queues[index] = (URLQueue*) malloc(sizeof(URLQueue));
        queues[index]->url = argv[URLArgIndices[index]];      // Copying pointer to each url to URLs array.
        printw("Added to queue: %s\n", queues[index]->url);
        queues[index]->filename = filenameFromURL(queues[index]->url);  // Default name
        if(optS || !optP)
        {
            queues[index]->nBytesToDownload = getSize(queues[index]->filename, queues[index]->url);                  // Prefetch file size.
            printw("File size fetch for %s: %lu\n", queues[index]->url, queues[index]->nBytesToDownload);
        }
        else
        {
            queues[index]->nBytesToDownload = 0;
        }
        refresh();
    }

    // Quicksort
    if(optS)
    {
        queueQuickSortDescending(queues, 0, numOfURLs - 1);
    }
    // ARGUMENT PARSING END

    // Status Struct Memory Allocation for each worker
    qStatusAllocated = true;
    statuses = (Status**) malloc(concurrentDownloadNum * sizeof(Status*));
    // Allocating each struct
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        // Allocate status struct for each worker.
        statuses[index] = (Status*) malloc(sizeof(Status));
        // Flag each worker as active
        statuses[index]->qWorkerActive = true;
    }

    // Allocate pointer to threads
    qThreadAllocated = true;
    threadPtr = (pthread_t*) malloc((concurrentDownloadNum + 1) * sizeof(pthread_t));
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        pthread_create(&threadPtr[index], NULL, queueWorker, statuses[index]);
    }
    pthread_create(&threadPtr[concurrentDownloadNum], NULL, workerStatViewer, NULL);





    // Wait until all threads are closed.
    for(unsigned int index = 0; index <= concurrentDownloadNum; index++)
    {
        pthread_join(threadPtr[index], NULL);
    }

    printw("Queue finished.\n");
    refresh();


    // Garbage Collection
    //free(URLs);

    for(int index = 0; index < numOfURLs; index++)
    {
        free(queues[index]);
    }
    free(queues);
    for(unsigned int index = 0; index < concurrentDownloadNum; index++)
    {
        free(statuses[index]);
    }
    free(statuses);
    free(threadPtr);
    free(URLArgIndices);

    // Curl Exit
    curl_global_cleanup();

    // ncurses end windows
    endwin();
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
            if(qURLArgIndicesAllocated == true)
            {
                free(URLArgIndices);
            }
            if(qQueuesAllocated == true)
            {
                for(int index = 0; index < numOfURLs; index++)
                {
                    free(queues[index]);
                }
                free(queues);
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
            endwin();
            exit(1);
            break;
    }
}

void showHelp()
{
    printf("Usage: octoget [options] url1 [url2] [url...]\n"
           "\n"
           "-c x\tSpecify the number of concurrent downloads (Default: 4)\n"
           "-s\tPrefetch the file sizes and sort to download larger files first.\n"
           "-p\tDo not prefetch the file size. (Fetching size when the file is on queue. (Ignored when -s is used)\n"
          );
    endwin();
    exit(1);
}
