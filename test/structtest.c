#include <stdio.h>
#include <stdlib.h>
#include "../src/queue.h"
#include "../src/structs.h"

int main()
{
    URLQueue** queues = (URLQueue**) malloc(5 * sizeof(URLQueue*));
    for(int i = 0; i < 5; i++)
    {
        queues[i] = (URLQueue*) malloc(sizeof(URLQueue));
    }
    queues[0]->nBytesToDownload = 3;
    queues[1]->nBytesToDownload = 1;
    queues[2]->nBytesToDownload = 4;
    queues[3]->nBytesToDownload = 5;
    queues[4]->nBytesToDownload = 2;

    queueQuickSortDescending(queues, 0, 4);


    free(queues);
}
