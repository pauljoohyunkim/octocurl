# Octocurl

Octocurl (formally, octoget) is a wrapper around CURL for bulk downloads.
If you have sufficient bandwidth, you can download multiple files, each with individual worker.

## How to use
> Usage: octocurl [options] url1 [url2] [url...]
>
> 
> -c x	Specify the number of concurrent downloads (Default: 4)
>
> -s	Prefetch the file sizes and sort to download larger files first.
>
> -p	Do not prefetch the file size. (Fetching size when the file is on queue. (Ignored when -s is used)

## Build
### Prerequisite
You need ncurses and pthreads library to compile this.

### Compilation (Building from Source)
Currently, installation is not automatic.

Compiling for development can be done by issuing
> make 

This creates the binary at bin/octocurl.

Compiling for standard use can be done by issuing
> make release

This also creates the binary at bin/octocurl.

## Testing
> \# You can test if it works by:
>
> make test
>
> \# You can clean the downloaded files from testing by
> make testclean

## Example
Downloading Linux from Scratch Packages (v11.2) as mentioned in https://linuxfromscratch.org/lfs/view/stable/chapter03/introduction.html using the command
> wget --input-file=wget-list-sysv --continue

It took 1 minute and 42.4695 seconds for all the packages to download.

On the other hand, using octocurl with the following command:
> bin/octocurl $(cat wget-list-sysv)

it took 29.0505 seconds. (gcc was the bottleneck here, so apart from gcc, everything downloaded quickly. Setting worker number to 25, it only took 22.076 seconds.)

Note that this sort of performance gain is due to using this program in a high bandwidth environment.

Here are results from more controlled trials (consistent background network activity, etc.):

| Commands          | Time          | Comment |
|---------------------------------------------------------------------------:|:---------:| ----------------------------------------------------------------|
| wget --input-file=wget-list-sysv --continue                                | 1m34.072s | Sequential downloading (at a later time)
| bin/octocurl $(cat wget-list-sysv)                                         | 0m29.048s | octocurl with 4 workers
| bin/octocurl -c 25 $(cat wget-list-sysv)                                   | 0m20.049s | octocurl with 25 workers
| bin/octocurl -c 86 \$(cat wget-list-sysv)                                  | 0m19.070s | octocurl with worker for each file (Note that, above certain number of workers, there isn't much speedup; rather you might even experience some performance drops.)
