typedef enum { BINARY_VECTOR, DOUBLE_VECTOR } vector_type;

typedef struct {
  char ** vector;
  char ** text;
  vector_type type;
  size_t vector_length;
  size_t n_entries;
} database;

database read_db_from_disk(char* filename);
void write_db_to_disk(char* filename, database db);

double cosine_distance(char* a, char* b, size_t vector_length);

size_t* create_closest_distances(database db,
				 double(*distance)(char*,char*,size_t),
				 char* query, size_t n_closest);

void print_binary_embeddings(char*a, size_t vector_length);
