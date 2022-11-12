#ifndef FILESTRING_H
#define FILESTRING_H

#define PLACEHOLDER_CHAR '*'

char* filenameFromURL(char* URLString);
int countChar(char c, char* string);
char* regexToFilename(char* URLString);

#endif
