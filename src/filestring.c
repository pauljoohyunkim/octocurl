#include <string.h>
#include <regex.h>
#include "filestring.h"


// Getting filename from URL
char* filenameFromURL(char* URLString)
{
    unsigned int offset = 0;
    
    for(unsigned int i = 0; i < strlen(URLString); i++)
    {
        if(URLString[i] == '/')
        {
            offset = i + 1;
        }
    }

    return URLString + offset;
}
