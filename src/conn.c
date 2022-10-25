#include <stdio.h>

#include "conn.h"

extern char** URLs;
extern unsigned int currentQueue;
extern pthread_mutex_t queueLock;

void* curlDownload(void* ptr)
{
    printf("Hello World!\n");

    return NULL;
}
