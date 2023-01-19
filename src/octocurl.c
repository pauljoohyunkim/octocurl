#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <curl/curl.h>
#include <ncurses.h>
#include <regex.h>

#include "octocurl.h"
#include "conn.h"
#include "queue.h"
#include "structs.h"
#include "filestring.h"

unsigned int concurrentDownloadNum = DEFAULT_CONCURRENT_DOWNLOAD;
unsigned int numOfURLs = 0;
unsigned int queueNum = 0;
URLQueue** queues;              // This replaced the array of URLs used before.
bool qQueuesAllocated = false;
pthread_t* threadPtr;           // Array of Threads
bool qThreadAllocated = false;  // Whether or not the threadPtr is allocated or not.
Status** statuses;              // Array of Status struct pointers for each worker.
bool qStatusAllocated = false;  // Status structs allocated
bool qCurlGlobalInitialized = false;   // Curl Global Initialization
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;      // Mutex Lock for Worker Queue Read
unsigned int* URLArgIndices;    // Indicies from argv which correspond to URL
bool qURLArgIndicesAllocated = false;
bool qFilenameAllocated = false;
int terminalWidth;                 // The columns and rows for ncurses
int y, x;                           // ncurses location

// Regex Stuff
regex_t regexFilter;                // Regex Type
char regexFilterString[BUFSIZ];     // Regular expression to search against.
char outputFilterString[BUFSIZ];    // Output filename with placeholder *

bool optS = false;              // Whether or not quicksort is to be used or not.
bool optP = false;              // Whether or not to prefetch file size.
bool optR = false;              // Regex search to use
bool optO = false;              // Output filename
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
    while((c = getopt(argc, argv, "hc:spr:o:")) != -1)
    {
        switch(c)
        {
            case 'h':
                showHelp();
            case 'c':
                ;
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
                    endwin();
                    fprintf(stderr, "The number of concurrent download workers should be between %d and %d\n", MIN_CONCURRENT_DOWNLOAD, MAX_CONCURRENT_DOWNLOAD);
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                optS = true;
                printw("Quicksort will be used to download larger files first.\n");
                break;
            case 'p':
                optP = true;
                printw("File size prefetch enabled.\n");
                break;
            case 'r':
                optR = true;
                strcpy(regexFilterString, optarg);

                // Compiling the regex expression; exit if error
                if(regcomp(&regexFilter, regexFilterString, 0))
                {
                    endwin();
                    fprintf(stderr, "There was a problem with the given regular expression.\n");
                    exit(1);
                }
                        
                printw("Regular expression to use: %s\n", regexFilterString);
                break;
            case 'o':
                optO = true;
                strcpy(outputFilterString, optarg);
                printw("Output file format with placeholder \'*\': %s\n", outputFilterString);
                break;
        }
    }

    // -r option has to be used with -o option
    if(optR && !optO)
    {
        endwin();
        fprintf(stderr, "In order to use regex functionality, you need to use both -r and -o options.\n");
        exit(1);
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
    if(optR && optO)
    {
        qFilenameAllocated = true;
    }
    for(int index = 0; index < numOfURLs; index++)
    {
        queues[index] = (URLQueue*) malloc(sizeof(URLQueue));
        queues[index]->url = argv[URLArgIndices[index]];      // Copying pointer to each url to URLs array.
        printw("Added to queue: %s\n", queues[index]->url);
        if(qFilenameAllocated)
        {
            // Using regex to assign filename
            queues[index]->filename = regexToFilename(queues[index]->url);
        }
        else
        {
            if(!optO)
            {
                queues[index]->filename = filenameFromURL(queues[index]->url);  // Default name
            }
            else
            {
                queues[index]->filename = outputFilterString;
            }
        }

        if(optS || optP)
        {
            queues[index]->nBytesToDownload = getSize(queues[index]->filename, queues[index]->url);                  // Prefetch file size.
            getyx(stdscr, y, x);
            if(y >= getmaxy(stdscr) - 2)
            {
                clear();
            }
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
    if(qFilenameAllocated == true)
    {
        for(int index = 0; index < numOfURLs; index++)
        {
            free(queues[index]->filename);
        }
    }

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
            if(qFilenameAllocated == true)
            {
                for(int index = 0; index < numOfURLs; index++)
                {
                    free(queues[index]->filename);
                }
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
    endwin();
    printf("Usage: octocurl [options] url1 [url2] [url...]\n"
           "\n"
           "-c x\t\t\tSpecify the number of concurrent downloads (Default: 4)\n"
           "-s\t\t\tPrefetch the file sizes and sort to download larger files first.\n"
           "-p\t\t\tDo not prefetch the file size. (Fetching size when the file is on queue. (Ignored when -s is used))\n"
           "-r <regex>\t\tRegular expression of each url. (This option requires -o option.)\n"
           "-o <filename form>\tForm of the output file name. (Use * for placeholder for the regex match)\n"
           "\n"
           "Examples:\n"
           "octocurl http://example.com\t\t\t\t\t\tDownloads the webpage and saves it as \'example.com\'\n"
           "octocurl -c 3 http://example.com/{1,2,3,4}.ts\t\t\t\tDownloads 1.ts, 2.ts, 3.ts, 4.ts with three workers.\n"
           "octocurl -s http://example.com/{1,2,3,4}.ts\t\t\t\tPrefetches the file size before downloading the contents and sorts\n"
           "\t\t\t\t\t\t\t\t\tthem (descending). This might be useful for downloading many files\n"
           "\t\t\t\t\t\t\t\t\tof potentially highly varying file sizes.\n"
           "octocurl -p http://example.com/{1,2,3,4}.ts\t\t\t\tFetches the file size when it is on queue.\n"
           "octocurl -r \'[0-9].ts\' -o \'./*\' http://vid.com/{1,2}.ts-foo-bar\t\tUses regex to save the files as 1.ts, 2.ts\n"
          );
    exit(1);
}
