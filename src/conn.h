#ifndef CONN_H
#define CONN_H

#include "structs.h"

void* queueWorker(void* ptr);
void* workerStatViewer(void* ptr);
void progressBar(float percentage, float speed);
int curlDownload(char* url, char* filename, Status* ptr);
size_t getData(char* buffer, size_t itemsize, size_t nitems, void* statusPtr);
curl_off_t getSize(char* filename, char* url);

// curlDownload return codes
#define CURLDOWN_GOOD 0
#define CURLDOWN_WRITEOPENERROR 1
#define CURLDOWN_DOWNLOADFAIL 2

#define STATUS_UPDATE_PERIOD 1

#endif
