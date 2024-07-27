#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "vector-db.h"
#include "load-texts.h"
#include "embedding-from-server.h"

int main(int argc, char** argv) {

  size_t i;
  
  database vdb;
  char** embeddings;
  char** text_slices;

  size_t n_slices;

  char* main_text;

  char** buffer;

  char* hostname = argv[2];
  char* port = argv[3];
  char* vdb_filename = argv[5];
  
  embedding e;

  size_t slices_lengths;

  if(argc < 5) {
    printf("Arguments are: \n"
	   " [file]     Text to slice and to store in vector database\n"
	   " [hostname] Hostname of the server providing embeddings\n"
	   " [port]     Port of the server providing embeddings\n"
	   " [int]      Number of characters for slices\n"
	   " [file]     Vector database to write\n");
    return(1);
  }
  
  buffer = texts_from_files(&(argv[1]),1);
  
  sscanf(argv[4],"%lu",&slices_lengths);

  main_text = buffer[0];

  text_slices = cut_texts_in_subtexts(main_text,
				      strlen(main_text),
				      slices_lengths,
				      &n_slices);

  embeddings = (char**)malloc(sizeof(char*)*n_slices);
  
  for(i=0;i<n_slices;i++) {
    e = get_embedding_from_server(hostname,port,text_slices[i]);
    posix_memalign((void**)(embeddings+i),
		   32,
		   sizeof(double)*(e.vector_length+(4-e.vector_length%4)));

    memset(embeddings[i],
	   0,
	   sizeof(double)*(e.vector_length+(4-e.vector_length%4)));

    memcpy(embeddings[i],e.vector,sizeof(double)*e.vector_length);

    free(e.vector);
  }

  vdb.vector = embeddings;
  vdb.text = text_slices;
  vdb.type = DOUBLE_VECTOR;
  vdb.vector_length = e.vector_length;
  vdb.n_entries = n_slices;

  write_db_to_disk(vdb_filename,vdb);

  return(0);
}

   
			    
