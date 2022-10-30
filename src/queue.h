#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include "structs.h"

void queueQuickSort(URLQueue** queues, unsigned int low, unsigned int high);
unsigned int queueQuickSortPartition(URLQueue** queues, unsigned int low, unsigned int high);
void queueQuickSortSwap(URLQueue** a, URLQueue** b);
#endif
