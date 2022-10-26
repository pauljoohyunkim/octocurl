# Octoget

Octoget is a wrapper around CURL for bulk downloads.
If you have sufficient bandwidth, you can download multiple files, each with multiple workers.

## How to use
> Usage: octoget [options] url1 [url2] [url...]
>
> -c x    Specify the number of concurrent downloads (Default: 4)

## How to compile
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
