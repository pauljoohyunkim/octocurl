#include "octoget.h"
#include "structs.h"
#include "queue.h"

void queueQuickSort(URLQueue** queues, unsigned int low, unsigned int high)
{
    if(low < high)
    {
        // Pivot
        unsigned int pi = queueQuickSortPartition(queues, low, high);

        // Left of Pivot
        queueQuickSort(queues, low, pi - 1);
        // Right of Pivot
        queueQuickSort(queues, pi + 1, high);
    }

}

unsigned int queueQuickSortPartition(URLQueue** queues, unsigned int low, unsigned int high)
{
    // Rightmost element as pivot
    unsigned int pivot = queues[high]->nBytesToDownload;

    unsigned int i = low - 1;

    for(unsigned int j = low; j < high; j++)
    {
        if(queues[j]->nBytesToDownload <= pivot)
        {
            i++;

            queueQuickSortSwap(&queues[i], &queues[j]);
        }
    }

    queueQuickSortSwap(&queues[i+1], &queues[high]);
    return (i+1);
}

void queueQuickSortSwap(URLQueue** a, URLQueue** b)
{
    URLQueue* t = *a;
    *a = *b;
    *b = t;

}
