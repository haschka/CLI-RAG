#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include"curl_helpers.h"


size_t write_function_callback(char* in_data,
			       size_t size,
			       size_t nmemb,
			       void* clientdata) {

  size_t totalsize = size * nmemb;
  response_data * data = (response_data *)clientdata;

  data->data = (char*)realloc(data->data , data->size + totalsize + 1);
  if(!data->data) {
    return 0;
  }

  memcpy(data->data+(data->size),in_data, totalsize);
  data->size += totalsize;
  data->data[data->size] = 0;

  return(totalsize);
}

size_t write_function_callback_stream_llm(char* in_data,
					  size_t size,
					  size_t nmemb,
					  void* clientdata) {

  response_data * data = (response_data*)clientdata;
  size_t totalsize = size * nmemb;

  struct json_object* response_line;
  struct json_object* content;

  const char* export_string;

  size_t export_string_length;

  
  response_line = json_tokener_parse(in_data+5);

  json_object_object_get_ex(response_line,"content",&content);

  if(content != NULL) {
  
    export_string_length = json_object_get_string_len(content);
    
    export_string = json_object_get_string(content);
    
    fputs(export_string,stdout);
    fflush(stdout);
    
    data->data = (char*)realloc(data->data ,
				data->size + export_string_length + 1);

    memcpy(data->data+data->size,export_string,export_string_length);
    
    data->size += export_string_length;
    data->data[data->size] = 0;
  }
  
  return(totalsize);
}
  
  
  
