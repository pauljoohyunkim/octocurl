#include <stdio.h>
#include <pthread.h>
#include <curl/curl.h>
#include "conn.h"

extern unsigned int concurrentDownloadNum;
extern int numOfURLs;
extern char** URLs;
extern unsigned int queueNum;
extern pthread_mutex_t queueLock;

// queueWorker takes void pointer and then casts to Status struct pointer.
void* queueWorker(void* ptr)
{
    Status* statusPtr = (Status*) ptr;
    unsigned int currentWorkerQueue;
    //printf("Hello World! %u %u\n", statusPtr->nBytesToDownload, statusPtr->nBytesDownloaded);
    while(1)
    {
        pthread_mutex_lock(&queueLock);
        if(queueNum == numOfURLs)        // Break out of the loop and finish worker if queueNum is equal to numOfURLs
        {
            pthread_mutex_unlock(&queueLock);
            break;
        }
        else
        {
            currentWorkerQueue = queueNum; // Otherwise assign an index for URLs to the current worker.
            queueNum++;
        }
        pthread_mutex_unlock(&queueLock);
        curlDownload(URLs[currentWorkerQueue], "test.txt", statusPtr);
    }

    return NULL;
}

int curlDownload(char* url, char* filename, Status* statusPtr)
{
    // curl handler initialization
    CURL* curl = curl_easy_init();
    if(!curl)
    {
        fprintf(stderr, "CURL initialization failed\n");
        return 1;
    }

    curl_off_t length;

    // curl handler option
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);  // Only get information, not the data.
    // Get length
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &length);

    // Initializing status
    statusPtr->nBytesToDownload = length;
    statusPtr->nBytesDownloaded = 0;
    //statusPtr->fp = fopen(filename, "wb");

    // Start downloading file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getData);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
    CURLcode result = curl_easy_perform(curl);
    printf("Something\n");

    curl_easy_cleanup(curl);
    //fclose(statusPtr->fp);

    return 0;
}

size_t getData(char* buffer, size_t itemsize, size_t nitems, void* ptr)
{
    size_t bytes = itemsize * nitems;
    Status* statusPtr = (Status*) ptr;
    //fwrite(buffer, itemsize, nitems, statusPtr->fp);
    
    printf("%s",buffer);
    return bytes;
}
