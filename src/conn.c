#include <stdio.h>
#include <curl/curl.h>
#include "conn.h"

extern char** URLs;

// queueWorker takes void pointer and then casts to Status struct pointer.
void* queueWorker(void* ptr)
{
    printf("Hello World!\n");

    return NULL;
}

void curlDownload(char* url, char* filename)
{
    
}
