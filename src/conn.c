#include <stdio.h>
#include <curl/curl.h>
#include "conn.h"

extern char** URLs;

// queueWorker takes void pointer and then casts to Status struct pointer.
void* queueWorker(void* ptr)
{
    Status* statusPtr = (Status*) ptr;
    //printf("Hello World! %u %u\n", statusPtr->nBytesToDownload, statusPtr->nBytesDownloaded);

    return NULL;
}

int curlDownload(char* url, char* filename, Status* ptr)
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
    ptr->nBytesToDownload = length;

    // Start downloading file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getData);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
    CURLcode result = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    return 0;
}

size_t getData(char* buffer, size_t itemsize, size_t nitems, void* statusPtr)
{
    size_t bytes = itemsize * nitems;
    printf("%s",buffer);
    return bytes;

}
