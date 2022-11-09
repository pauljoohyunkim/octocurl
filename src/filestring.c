#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "filestring.h"

extern regex_t regexFilter;
extern outputFilterString[];

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

// Counts the number of occurrences of a specific character.
int countChar(char c, char* string)
{
    int count = 0;
    for(int i = 0; i < strlen(string); i++)
    {
        if(string[i] == c)
        {
            count++;
        }
    }
    return count;
}

char* regexToFilename(char* URLString)
{
    regmatch_t match;               // This holds beginning and end (offsets) for the match
    int substituteStringLength;
    int placeholderCount = countChar(PLACEHOLDER_CHAR, outputFilterString);
    int copyIndex = 0;
    char* filenamePointer;
    int filenameLength;

    regexec(&regexFilter, URLString, 1, &match, 0);
    substituteStringLength = match.rm_eo - match.rm_so;
    // filename length is the length of the output filter string with * replaced with substitute string length
    // For example, if outputFilterString is "foo/*.test" and regex match comes out to "bar", then
    // the string constructed as filename should be foo/bar.test
    // The length computation according to statement below is 12 = 10 - 1 + 3 * 1
    filenameLength = strlen(outputFilterString) - placeholderCount + substituteStringLength * placeholderCount;
    
    // Allocate the filename array with extra space for null character.
    filenamePointer = (char*) calloc(filenameLength + 1, sizeof(char));

    // Copy the contents, replacing * for the regex match
    for(int index = 0; index < strlen(outputFilterString); index++)
    {
        if(outputFilterString[index] != PLACEHOLDER_CHAR)
        {
            filenamePointer[copyIndex] = outputFilterString[index];
            copyIndex++;
        }
        else
        {
            strncpy(filenamePointer + copyIndex, URLString + match.rm_so, substituteStringLength);
            copyIndex += substituteStringLength;
        }
    }

    return filenamePointer;
}
