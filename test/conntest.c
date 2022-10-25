#include <stdio.h>
#include <curl/curl.h>
#include "../src/conn.h"

int main()
{
    Status status;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curlDownload("http://127.0.0.1:8000/test.txt", "something", &status);
    curl_global_cleanup();
    return 0;
}
