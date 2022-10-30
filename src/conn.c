#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <curl/curl.h>
#include <string.h>
#include <ncurses.h>

#include "conn.h"
#include "octoget.h"

extern unsigned int concurrentDownloadNum;
extern unsigned int numOfURLs;
//extern char** URLs;
extern URLQueue** queues;

extern Status** statuses;

extern unsigned int queueNum;       // This keeps track of which url to produce next.
extern pthread_mutex_t queueLock;

extern int terminalWidth;              // ncurses terminal size

// queueWorker takes void pointer and then casts to Status struct pointer.
void* queueWorker(void* ptr)
{
    Status* statusPtr = (Status*) ptr;
    unsigned int currentWorkerQueue;

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
        //statusPtr->url = URLs[currentWorkerQueue];
        statusPtr->url = queues[currentWorkerQueue]->url;
        statusPtr->filename = filenameFromURL(statusPtr->url);
        statusPtr->nBytesDownloadedPerIter = 0;
        printw("Downloading %s as %s\n", statusPtr->url, statusPtr->filename);
        curlDownload(statusPtr->url, statusPtr->filename, statusPtr);
    }

    // Flag worker as inactive
    // This is used for the workerStatViewer thread to monitor whether or not all the workers
    // are active or not.
    statusPtr->qWorkerActive = false;
    return NULL;
}

// Shows progress of each worker
void* workerStatViewer(void* ptr)
{
    // Checking if all workers are inactive.
    bool areAllWorkersInactive = false;
    while(STATUS_UPDATE_PERIOD)
    {
        sleep(1);
        clear();            // clear ncurses screen
        terminalWidth = getmaxx(stdscr);    // gets terminal width to take changing window size into account.
        // Check if all workers are inactive.
        // Iterate through statuses and if one of them are true, one of the workers is active,
        // hence areAllWorkersInactive is false.
        // Otherwise, it is true.
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
            // Display progress.
            for(int index = 0; index < concurrentDownloadNum; index++)
            {
                // Only show active workers
                if(statuses[index]->qWorkerActive)
                {
                    printw("Worker %i (%s) : %s\n", index, statuses[index]->filename, statuses[index]->url);
                    // Division by Zero
                    if(statuses[index]->nBytesToDownload)
                    {
                        float percentage = ((float) (statuses[index]->nBytesDownloaded)) / (statuses[index]->nBytesToDownload) * 100;
                        progressBar(percentage, (float) (statuses[index]->nBytesDownloadedPerIter) / STATUS_UPDATE_PERIOD);
                    }
                }
            }
            refresh();
            continue;
        }
        else
        {
            break;
        }
    }

    printw("All workers finished!\n");
    refresh();

    return NULL;
}

// Prints a terminal progress bar
void progressBar(float percentage, float speed)
{
    int progressBarLength = (int) floor(terminalWidth * 0.6);
    int numOfStars = (int) floor(percentage * progressBarLength / 100);
    printw("\t[");
    for(int i = 0; i < numOfStars; i++)
    {
        printw("*");
    }
    for(int i = 0; i < progressBarLength - numOfStars; i++)
    {
        printw(" ");
    }
    printw("]\t [%.2f%%]\t", percentage);
    if(speed / 1000 < 1)
    {
        printw("%.2f B/s\n", speed);
    }
    else if(speed / 1000000 < 1)
    {
        printw("%.2f kB/s\n", speed / 1000);
    }
    else if(speed / 1000000000 < 1)
    {
        printw("%.2f mB/s\n", speed / 1000000);
    }
    else
    {
        printw("%.2f gB/s\n", speed / 1000000000);
    }
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
        fprintf(stderr, "Download failed for %s from %s. CURLCODE=%d\n", filename, url, curlcode);
        curl_easy_cleanup(curl);
        fclose(statusPtr->fp);
        return CURLDOWN_DOWNLOADFAIL;
    }

    curl_easy_cleanup(curl);
    fclose(statusPtr->fp);
    printw("Downloaded %s as %s\n", url, filename);
    refresh();

    return 0;
}

size_t getData(char* buffer, size_t itemsize, size_t nitems, void* ptr)
{
    size_t bytes = itemsize * nitems;
    Status* statusPtr = (Status*) ptr;
    statusPtr->nBytesDownloaded += bytes;
    statusPtr->nBytesDownloadedPerIter += bytes;
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
