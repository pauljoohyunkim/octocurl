#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "octoget.h"


int main(int argc, char* argv[])
{
    // The threads for downloading individual files.
    //pthread_t* workers;
    
    int c;              // Option
    unsigned int concurrentDownloadNum = DEFAULT_CONCURRENT_DOWNLOAD;
    unsigned int numOfURLs = 0;
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

    // Iterate to count the numbers of URLs parsed through command line
    for(; optind < argc; optind++)
    {
        numOfURLs++;
    }
    optind = 1;         // Reset optind to iterate through the extra arguments again.
    for(; optind < argc; optind++)
    {
        printf("%s\n", argv[optind]);
    }



    return 0;
}

void showHelp()
{
    printf("Usage: octoget [options] url1 [url2] [url...]\n"
           "\n"
           "-c x\tSpecify the number of concurrent downloads (Default: 4)\n"
          );
}
