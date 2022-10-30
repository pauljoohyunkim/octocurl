#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include "structs.h"

void queueQuickSortDescending(URLQueue** queues, int low, int high);
void queueQuickSort(URLQueue** queues, int low, int high);
int queueQuickSortPartition(URLQueue** queues, int low, int high);
void queueQuickSortSwap(URLQueue** a, URLQueue** b);
#endif
