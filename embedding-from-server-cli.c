#include<stdio.h>

#include"embedding-from-server.h"

int main(int argc, char** argv) {

  size_t i;
  
  embedding e = get_embedding_from_server(argv[1],argv[2],argv[3]);

  for(i=0;i<e.vector_length;i++) {

    printf("%10.8lf\n", e.vector[i]);

  }
  return(0);
}
  
