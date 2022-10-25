#ifndef CONN_H
#define CONN_H

typedef struct status
{
    unsigned int nBytesToDownload;
    unsigned int nBytesDownloaded;
} Status;

void* queueWorker(void* ptr);
int curlDownload(char* url, char* filename, Status* ptr);
size_t getData(char* buffer, size_t itemsize, size_t nitems, void* statusPtr);

#endif
