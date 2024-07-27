/*! \file binary_array.c						      	
 *  \brief This file contains functions to set and get values from binary arrays
 */
#include<stdlib.h>
#include<string.h>

char* alloc_and_set_zero_binary_array(size_t array_length) {

  char* bm = (char*)malloc(sizeof(char*)*((array_length/8+1)));

  if(bm == NULL) {
    return NULL;
  }

  memset(bm,0,sizeof(char*)*((array_length/8+1)));
  return(bm);
}

char* set_zero_binary_array(char* b_array,size_t array_length) {
  return((char*)(memset(b_array,0,sizeof(char*)*(array_length/8+1))));
}

char* alloc_binary_array(size_t array_length) {
  char* bm = (char*)malloc(sizeof(char*)*((array_length/8+1)));
  return(bm);
}

void binary_array_or(char* result, size_t array_length, char* a, char* b) {
  int i;
  for(i=0;i<array_length/8+1;i++) {
    result[i] = a[i] | b[i];
  }
}

void binary_array_and(char* result, size_t array_length, char* a, char* b) {
  int i;
  for(i=0;i<array_length/8+1;i++) {
    result[i] = a[i] & b[i];
  }
}


void set_value_in_binary_array_at_index(char* b_array,
					 size_t index) {

  char small_index;
  int module = index%(sizeof(char)*8);
  size_t location = index/(sizeof(char)*8);

  small_index = (char)1 << module;

  b_array[location] |= small_index;
}

int get_value_in_binary_array_at_index(char* b_array,
					 size_t index) {

  char small_index;
  int module = index%(sizeof(char)*8);
  size_t location = index/(sizeof(char)*8);

  small_index = (char)1 << module;

  return(small_index == (b_array[location] & small_index));
}
