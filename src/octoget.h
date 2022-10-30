#ifndef OCTOGET_H
#define OCTOGET_H

#define DEFAULT_CONCURRENT_DOWNLOAD 4
#define MIN_CONCURRENT_DOWNLOAD     1
#define MAX_CONCURRENT_DOWNLOAD     100

typedef struct
{
    char* url;
    char* filename;
    char* nBytesToDownload;
} URLQueue;

void handler(int);
void showHelp();

#endif
