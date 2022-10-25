#include <stdio.h>
#include <curl/curl.h>
#include "conn.h"

extern char** URLs;

// queueWorker takes void pointer and then casts to Status struct pointer.
void* queueWorker(void* ptr)
{
    Status* statusPtr = (Status*) ptr;
    printf("Hello World! %u %u\n", statusPtr->nBytesToDownload, statusPtr->nBytesDownloaded);

    return NULL;
}

void curlDownload(char* url, char* filename)
{
    
}
