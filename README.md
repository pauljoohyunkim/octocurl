# Octoget

Octoget is a wrapper around CURL for bulk downloads. (I know, it is a bit misleading, but I didn't think about the name as hard as much as I spent time coding this up.)
If you have sufficient bandwidth, you can download multiple files, each with individual worker.

## How to use
> Usage: octoget [options] url1 [url2] [url...]
>
> -c x    Specify the number of concurrent downloads (Default: 4)

## Build
### Prerequisite
You need ncurses and pthreads library to compile this.

### Compilation (Building from Source)
Currently, installation is not automatic.

Compiling for development can be done by issuing
> make 

This creates the binary at bin/octoget.

Compiling for standard use can be done by issuing
> make release

This also creates the binary at bin/octoget.

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

On the other hand, using octoget with the following command:
> bin/octoget $(cat wget-list-sysv)

it took 29.0505 seconds. (gcc was the bottleneck here, so apart from gcc, everything downloaded quickly. Setting worker number to 25, it only took 22.076 seconds.)

Note that this sort of performance gain is due to using this program in a high bandwidth environment.
