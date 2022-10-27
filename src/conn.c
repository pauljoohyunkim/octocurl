#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <curl/curl.h>
#include <string.h>

#include "conn.h"

extern unsigned int concurrentDownloadNum;
extern int numOfURLs;
extern char** URLs;

extern Status** statuses;

extern unsigned int queueNum;
extern pthread_mutex_t queueLock;

// queueWorker takes void pointer and then casts to Status struct pointer.
void* queueWorker(void* ptr)
{
    Status* statusPtr = (Status*) ptr;
    unsigned int currentWorkerQueue;
    //printf("Hello World! %u %u\n", statusPtr->nBytesToDownload, statusPtr->nBytesDownloaded);

    // Flag worker as active
    statusPtr->qWorkerActive = true;
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
        printf("Downloading %s as %s\n", URLs[currentWorkerQueue], filenameFromURL(URLs[currentWorkerQueue]));
        curlDownload(URLs[currentWorkerQueue], filenameFromURL(URLs[currentWorkerQueue]), statusPtr);
    }

    // Flag worker as inactive
    statusPtr->qWorkerActive = false;
    return NULL;
}

// Shows progress of each worker
void* workerStatViewer(void* ptr)
{
    bool areAllWorkersInactive = false;
    while(1)
    {
        sleep(1);
        // Check if all workers are inactive.
        for(int index = 0; index < concurrentDownloadNum; index++)
        {
            if(statuses[index]->qWorkerActive == true)
            {
                areAllWorkersInactive = false;
                break;
            }
            areAllWorkersInactive = true;
        }
        if(areAllWorkersInactive == false)
        {
            continue;
        }
        else
        {
            break;
        }
        //for(int index = 0; index < concurrentDownloadNum; index++)
        //{
        //    isAllWorkerDone = isAllWorkerDone && statuses[index]->qWorkerActive;
        //}
        //isAllWorkerDone = !isAllWorkerDone;
    }

    printf("Done!\n");
    return NULL;
}

int curlDownload(char* url, char* filename, Status* statusPtr)
{
    curl_off_t length;      // Size of the file that will be downloaded.
    CURLcode curlcode;      // Curl return value

    // curl handler initialization
    CURL* curl = curl_easy_init();
    if(!curl)
    {
        fprintf(stderr, "CURL initialization failed\n");
        return 1;
    }


    // curl handler option
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);  // Only get information, not the data.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, statusPtr);

    // Get length
    curlcode = curl_easy_perform(curl);
    if(curlcode != CURLE_OK)
    {
        fprintf(stderr, "Could not get file size for %s from %s.", filename, url);
    }
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &length);

    // Initializing status
    statusPtr->nBytesToDownload = length;
    statusPtr->nBytesDownloaded = 0;
    statusPtr->fp = fopen(filename, "wb");
    if(statusPtr->fp == NULL)
    {
        fprintf(stderr, "Cannot open %s for writing. Check if you have write permission on the directory?\n", filename);
    curl_easy_cleanup(curl);
    return CURLDOWN_WRITEOPENERROR;
    }

    // Start downloading file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getData);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
    curlcode = curl_easy_perform(curl);

    if(curlcode != CURLE_OK)
    {
        fprintf(stderr, "Download failed for %s from %s\n", filename, url);
        curl_easy_cleanup(curl);
        fclose(statusPtr->fp);
        return CURLDOWN_DOWNLOADFAIL;
    }

    curl_easy_cleanup(curl);
    fclose(statusPtr->fp);
    printf("Downloaded %s as %s\n", url, filename);

    return 0;
}

size_t getData(char* buffer, size_t itemsize, size_t nitems, void* ptr)
{
    size_t bytes = itemsize * nitems;
    Status* statusPtr = (Status*) ptr;
    fwrite(buffer, itemsize, nitems, statusPtr->fp);
    
    //printf("%s",buffer);
    return bytes;
}

// Getting filename from URL
char* filenameFromURL(char* URLString)
{
    unsigned int offset = 0;
    
    for(unsigned int i = 0; i < strlen(URLString); i++)
    {
        if(URLString[i] == '/')
        {
            offset = i + 1;
        }
    }

    return URLString + offset;
}
