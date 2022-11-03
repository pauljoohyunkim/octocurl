#include "octocurl.h"
#include "structs.h"
#include "queue.h"

void queueQuickSortDescending(URLQueue** queues, int low, int high)
{
   int i = low;
   int j = high;

   queueQuickSort(queues, i, j);

   while(i < j)
   {
        queueQuickSortSwap(&queues[i], &queues[j]);
        i++;
        j--;
   }
}

void queueQuickSort(URLQueue** queues, int low, int high)
{
    if(low < high)
    {
        // Pivot
        int pi = queueQuickSortPartition(queues, low, high);

        // Left of Pivot
        queueQuickSort(queues, low, pi - 1);
        // Right of Pivot
        queueQuickSort(queues, pi + 1, high);
    }

}

int queueQuickSortPartition(URLQueue** queues, int low, int high)
{
    // Rightmost element as pivot
    int pivot = queues[high]->nBytesToDownload;

    int i = low - 1;

    for(int j = low; j < high; j++)
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
