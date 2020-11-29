#include "mycurl.h"





static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp)
 {
   size_t realsize = size * nmemb;
   struct write_memory *mem = (struct write_memory *)userp;
   char *ptr;
   ptr = (char*)realloc(mem->memptr, mem->size + realsize + 1);
   if(ptr == NULL)
     return 0;  /* out of memory! */
   mem->memptr = ptr;
   memcpy(&(mem->memptr[mem->size]), data, realsize);
   mem->size += realsize;
   mem->memptr[mem->size] = 0;

   return realsize;
 }

static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
  struct read_memory *read_mem = (struct read_memory *)userp;
  size_t buffer_size = size*nmemb;

  if(read_mem->sizeleft) {
    /* copy as much as possible from the source to the destination */
    size_t copy_this_much = read_mem->sizeleft;
    if(copy_this_much > buffer_size)
      copy_this_much = buffer_size;
    memcpy(dest, read_mem->memptr, copy_this_much);

    read_mem->memptr += copy_this_much;
    read_mem->sizeleft -= copy_this_much;
    return copy_this_much; /* we copied this many bytes */
  }

  return 0; /* no more data left to deliver */
}


size_t curl(string methode,string url,string data,write_memory* response,string auth_header)
{
  CURL *curl;
  CURLcode res;
  struct curl_slist *list = NULL;
  struct read_memory read_mem;

  char *char_url = NULL;
  stringtochar(url,char_url);

  char *char_data = NULL;
  stringtochar(data,char_data);

  char *char_methode = NULL;
  stringtochar(methode,char_methode);

  read_mem.memptr = char_data;
  read_mem.sizeleft = strlen(char_data);




  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, char_url);
    /* Now specify the POST data */
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");

    if(methode.compare("POST")!=0 && methode.compare("GET")!=0){
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, char_methode);
    }
    if(methode.compare("GET")==0){

            /* Now specify we want to POST data */
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
    }
    if(methode.compare("POST")==0){

            /* Now specify we want to POST data */
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
            /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

            /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_READDATA, (void*)&read_mem);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)read_mem.sizeleft);
    }

    /* get verbose debug output please */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);



    /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

    if(auth_header.compare("") != 0){
         char *char_auth = (char*)malloc(1);
        string header = "X-Fbx-App-Auth:" + auth_header;
        stringtochar(header,char_auth);
        list = curl_slist_append(list, char_auth);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    }

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

//cout<<endl<<endl<<"REPONSE :"<<response->memptr<<endl;
  return 0;
}
