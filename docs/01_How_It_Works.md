# How Octocurl Works

Honestly, there is not much to say as it is rather straightforward.
In a nutshell, octocurl parses urls that are given and forms a queue (see URLQueue from [src/structs.h](../src/structs.h)).

If sort option (-s) is enabled, octocurl fetches the file size for each of the URL parsed, then quicksorts the queue such that larger files have priority. (Note that this will add an overhead in the beginning.)

Octocurl then generates "workers" (see Status from [src/struct.h](../src/structs.h)) which are threads which process the task they are given.
Each worker takes in a task from the queue formed, downloads the file, then takes in the next item in the queue.

While there are any workers that are active, statuses of each worker will be shown by one additional thread. (see workerStatViewer [src/conn.c](../src/conn.c))