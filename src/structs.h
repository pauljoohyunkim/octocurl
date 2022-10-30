#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Each worker recycles Status assigned.
typedef struct
{
    size_t nBytesToDownload;
    size_t nBytesDownloaded;
    size_t nBytesDownloadedPerIter;
    char* url;
    char* filename;
    bool qWorkerActive;
    FILE* fp;
} Status;


// Each item in queue has url, filename, nBytesToDownload.
// Note that nBytesToDownload is used to optimize queue by quicksort.
typedef struct
{
    char* url;
    char* filename;
    size_t nBytesToDownload;
} URLQueue;

#endif
