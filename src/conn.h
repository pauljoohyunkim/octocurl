#ifndef CONN_H
#define CONN_H

// Each worker recycles Status assigned.
typedef struct status
{
    unsigned int nBytesToDownload;
    unsigned int nBytesDownloaded;
    FILE* fp;
} Status;

void* queueWorker(void* ptr);
int curlDownload(char* url, char* filename, Status* ptr);
size_t getData(char* buffer, size_t itemsize, size_t nitems, void* statusPtr);
char* filenameFromURL(char* URLString);

// curlDownload return codes
#define CURLDOWN_GOOD 0
#define CURLDOWN_WRITEOPENERROR 1
#define CURLDOWN_DOWNLOADFAIL 2

#endif
