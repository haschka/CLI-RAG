#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

char** texts_from_files(char** files, int n_files) {
  int i;
  int fd;
  char fileindex[11];

  char * filename;

  size_t textsize;

  char ** textdb = (char**)malloc(sizeof(char*)*n_files);
  
  for(i=0;i<n_files;i++) {

    fd = open(files[i], O_RDONLY);

    textsize = lseek(fd, 0 ,SEEK_END);
    textdb[i] = (char*)malloc(sizeof(char)*textsize+1);

    lseek(fd,0,SEEK_SET);
    
    if(textsize != read(fd, textdb[i], textsize)) {
      printf("Error reading textfiles at file %s",filename);
      _exit(1);
    }
    textdb[i][textsize]=0;
    close(fd);
  }
  return(textdb);
}
    
char** cut_texts_in_subtexts(char* text,
			     size_t text_length,
			     size_t requested_length,
			     size_t* n_texts){

  size_t previous_position = 0;
  size_t position = requested_length;
  size_t texts_counter = 0;
  
  char** texts = NULL;
  
  if (text_length < requested_length) {
    texts = (char**)malloc(sizeof(char*));

    texts[0] = (char*)malloc(sizeof(char)*(text_length+1));

    memcpy(texts[0],text,sizeof(char)*text_length);
    texts[0][text_length] = 0;
    n_texts[0] = 1;
    return(texts);
  }

  while(position < text_length - 1) {

    while(text[position] != ' ' && position > previous_position) {
      position--;
    }

    if(position == previous_position) {
      fprintf(stderr, "No new words found on interval at cutting texts");
      position += requested_length;
    }

    texts = (char**)realloc(texts,sizeof(char*)*(texts_counter+1));
    texts[texts_counter] =
      (char*)malloc(sizeof(char)*(position-previous_position+1));
    
    memcpy(texts[texts_counter],
	   text+previous_position,
	   sizeof(char)*position-previous_position);

    texts[texts_counter][position-previous_position] = 0;
    texts_counter++;

    
      
    previous_position = position;
    position += requested_length;
  }

  if(previous_position < text_length && position >= text_length - 1) {

    position = text_length - 1;

    texts = (char**)realloc(texts,sizeof(char*)*(texts_counter+1));
    texts[texts_counter] =
      (char*)malloc(sizeof(char)*(position-previous_position+1));
    
    memcpy(texts[texts_counter],
	   text+previous_position,
	   sizeof(char)*position-previous_position);

    texts[texts_counter][position-previous_position] = 0;
    texts_counter++;
  }

  n_texts[0] = texts_counter; 
  return(texts);
}
      
    
    

    
    
      
			     

  
  

  
    
    
