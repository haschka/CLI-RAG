typedef struct {
  double* vector;
  size_t vector_length;
} embedding;

embedding get_embedding_from_server(char* hostname, char* port, char* text);

