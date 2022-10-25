#ifndef CONN_H
#define CONN_H

typedef struct status
{
    unsigned int nBytesToDownload;
    unsigned int nBytesDownloaded;
} Status;

void* queueWorker(void* ptr);
void curlDownload(char* url);

#endif
