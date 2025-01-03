#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>
#include <json-c/json.h>
#include "local_resolve.h"
#include "curl_helpers.h"

typedef struct {
  double* vector;
  size_t vector_length;
} embedding;

char* generate_embedding_query(char* text) {

  struct json_object* new_query = json_object_new_object();

  char* return_string;
  size_t return_length;

  const char* return_string_pointer;

  json_object_object_add(new_query,
			 "content",
			 json_object_new_string(text));

  return_string_pointer = json_object_to_json_string(new_query);
  return_length = strlen(return_string_pointer);

  return_string = (char*)malloc(sizeof(char)*(return_length+1));

  memcpy(return_string,return_string_pointer,return_length);

  return_string[return_length] = 0;

  return(return_string);
}

embedding generate_embedding_from_server_data(char* server_response) {

  size_t i;

  struct json_object* response_j = json_tokener_parse(server_response);

  struct json_object* embedding_array_j;
  struct json_object* embedding_vector_item;

  struct json_object* response_j_inner;
  struct json_object* embedding_array_j_outher;

  
  embedding e;

  int embedding_length;

  size_t vector_write_size;

  if(response_j == NULL) {
    fprintf(stderr,
	    "Full JSON reponse from EMBEDDING "
	    "is not a valid json_object\n");
    _exit(1);
  }

  response_j_inner = json_object_array_get_idx(response_j,0);

  if(response_j == NULL) {
    fprintf(stderr,
	    "Full JSON response from EMBEDDING "
	    "is not an array, and first item could not be extracted\n"
	    "The servers repsonse is given below \n%s\n", server_response);
    _exit(1);
  }
  
  json_object_object_get_ex(response_j_inner, "embedding",
			    &embedding_array_j_outher);

  embedding_array_j = json_object_array_get_idx(embedding_array_j_outher,0);

  if( embedding_array_j == NULL ) {
    fprintf(stderr,
	    "Server response does not contain an embedding.\n"
	    "The servers repsonse is given below \n%s\n", server_response);
    _exit(1);
  }

  embedding_length = json_object_array_length(embedding_array_j);

  vector_write_size = sizeof(double)*embedding_length;
  
  posix_memalign((void**)(&(e.vector)),
		 32,vector_write_size+(32-vector_write_size%32)); 

  memset(e.vector,0,vector_write_size+(32-vector_write_size%32));
  
  e.vector_length = embedding_length;
  
  for(i=0;i<embedding_length;i++) {
    embedding_vector_item = json_object_array_get_idx(embedding_array_j,i);
    e.vector[i] = json_object_get_double(embedding_vector_item);
  }
  return(e);
}

embedding get_embedding_from_server(char* hostname, char* port, char* text) {

  CURL *curl = curl_easy_init();

  struct curl_slist *host = NULL;
  char* curl_slist_string;
  size_t curl_slist_string_size;

  char* ip_address;
  
  CURLcode result;

  response_data rd;

  int terminate = 0;

  char* url;
  size_t url_size;

  embedding e;

  char* embedding_query = generate_embedding_query(text);

  url_size = strlen(hostname)+strlen(port) + 24 ;
  url = (char*)malloc(sizeof(char)*url_size);
  sprintf(url,"http://%s:%s/embedding",hostname,port);

  rd.size = 0;
  rd.data = NULL;

  if(!curl) {
    fprintf(stderr,"Failed to initialize CURL, EXITING\n");
    _exit(1);
  }

  ip_address = local_resolve(hostname);

  if(ip_address != NULL) {

    curl_slist_string_size = strlen(hostname)
      + strlen(port) + strlen(ip_address) + 3;
    
    curl_slist_string = (char*)malloc(sizeof(char)*curl_slist_string_size);
    sprintf(curl_slist_string,"%s:%s:%s",hostname,port,ip_address);

    host = curl_slist_append(NULL,curl_slist_string);

    curl_easy_setopt(curl, CURLOPT_RESOLVE,host);
  }

  curl_easy_setopt(curl, CURLOPT_URL,url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,(long)strlen(embedding_query));
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS,embedding_query);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&rd);

  result = curl_easy_perform(curl);

  e = generate_embedding_from_server_data(rd.data);

  if(ip_address != NULL) free(ip_address);
  curl_easy_cleanup(curl);

  return(e);
}

  
		   

		 

	      
  
  
