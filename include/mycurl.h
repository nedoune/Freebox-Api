#ifndef MYCURL_H_INCLUDED
#define MYCURL_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <cstring>

#include "utils.h"

using namespace std;

struct write_memory {
  char *memptr;
  size_t size;
};


struct read_memory {
  const char *memptr;
  size_t sizeleft;
};


size_t curl(string methode,string url,string data,write_memory* response,string auth_header="");


#endif // CURL_H_INCLUDED
