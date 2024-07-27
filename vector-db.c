#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<float.h>

#if defined(__AVX2__)
#include<immintrin.h>
#endif

#include"vector-db.h"
#include"binary_array.h"

typedef struct {
  double distance;
  size_t index;
} distances;

int d_comp(const void* a,const void *b) {
  distances* a_d = (distances*)a;
  distances* b_d = (distances*)b;
  if(a_d[0].distance < b_d[0].distance) {
    return -1;
  } else if (a_d[0].distance > b_d[0].distance) {
    return 1;
  }
  return 0;
}

static inline void closest_rotate(size_t* closest_distances,
				  size_t new_closest,
				  size_t n_closest) {

  int i;
  for(i=n_closest-2;i>=0;i--) {
    closest_distances[i+1] = closest_distances[i];
  }
  closest_distances[0] = new_closest;
}

void write_db_to_disk(char* filename, database db) {

  char desc[4] = "VEC";

  int fd = open(filename,
		O_CREAT | O_WRONLY,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

  size_t i;

  size_t write_vector_length;

  size_t current_text_length;

  char t;
  
  if(db.type == BINARY_VECTOR) {
    write_vector_length = db.vector_length/8+1;
    t = 0;
  } else if(db.type == DOUBLE_VECTOR) {
    t = 1;
    write_vector_length = db.vector_length*sizeof(double);
  }

  write(fd,desc,sizeof(char)*4);
  write(fd,&t,sizeof(char));
  write(fd,&db.n_entries,sizeof(size_t));
  write(fd,&db.vector_length,sizeof(size_t));
  
  for(i=0;i<db.n_entries;i++) {
    write(fd,db.vector[i],write_vector_length);
  }
  for(i=0;i<db.n_entries;i++) {
    current_text_length = strlen(db.text[i]);
    write(fd,&current_text_length,sizeof(size_t));
    write(fd,db.text[i],current_text_length);
  }
}

database read_db_from_disk(char* filename) {
  
  char desc[4] = "VEC";

  char* check_desc = (char*)malloc(sizeof(char)*4);
  
  int fd = open(filename,O_RDONLY );

  size_t i;

  size_t write_vector_length;

  size_t current_text_length;

  char t;

  database db;
  
  memset(check_desc,0,sizeof(char)*4);
  

  read(fd,check_desc,sizeof(char)*4);
  if(0 !=strcmp(desc,check_desc)) {
    fprintf(stderr,"Warning Database file to be load corrupted!");
  }
  
  read(fd,&t,sizeof(char));
  read(fd,&db.n_entries,sizeof(size_t));
  read(fd,&db.vector_length,sizeof(size_t));

  if(t == 0) {
    write_vector_length = db.vector_length/8+1;
    db.type = BINARY_VECTOR;
  } else if(t == 1) {    
    write_vector_length = db.vector_length*sizeof(double);
    db.type = DOUBLE_VECTOR;
  }

  db.vector = (char**)malloc(sizeof(char*)*db.n_entries);
  db.text = (char**)malloc(sizeof(char*)*db.n_entries);
  
  for(i=0;i<db.n_entries;i++) {
    if(db.type == BINARY_VECTOR) {
      posix_memalign((void**)(db.vector+i),32,
		     sizeof(char)*(write_vector_length
				   +(32-write_vector_length%32)));
      memset(db.vector[i],0,
	     write_vector_length+(32-write_vector_length%32));
    } else if(db.type == DOUBLE_VECTOR) {
      posix_memalign((void**)(db.vector+i),32,
		     write_vector_length+(32-write_vector_length%32));
      memset(db.vector[i],0,
	     write_vector_length+(32-write_vector_length%32));
      
    }
    read(fd,db.vector[i],write_vector_length);
  }
  
  for(i=0;i<db.n_entries;i++) {
    read(fd,&current_text_length,sizeof(size_t));
    db.text[i] = (char*)malloc(sizeof(char)*(current_text_length+1));
    read(fd,db.text[i],current_text_length);
    db.text[i][current_text_length]=0;
  }
  return(db);
}

size_t* create_closest_distances(database db,
				 double(*distance)(char*,char*,size_t),
				 char* query, size_t n_closest) {
  
  int i;
  size_t* closest_distances = (size_t*)malloc(sizeof(size_t)*n_closest);

  distances* d = (distances*)malloc(sizeof(distances)*db.n_entries);
  
  char ** vectors = db.vector;
  size_t n_entries = db.n_entries;
  
  if(db.n_entries < n_closest) {
    fprintf(stderr,"Less database entries than closest distances searched!");
    _exit(1);
  }   

  for(i=0;i<db.n_entries;i++) {
    d[i].distance = distance(query,vectors[i],db.vector_length);
    d[i].index = i;
  }

  qsort(d,db.n_entries,sizeof(distances),d_comp);

  for(i=0;i<n_closest;i++) {
    closest_distances[i] = d[i].index;
  } 
  free(d);
  return(closest_distances);
}

#if defined(__AVX2__)

double cosine_distance(char* a, char* b, size_t vector_length) {

  double* a_d = (double*)a;
  double* b_d = (double*)b;

  __m256d a_in_b_vec = _mm256_setzero_pd();
  __m256d a_in_a_vec = _mm256_setzero_pd();
  __m256d b_in_b_vec = _mm256_setzero_pd();

  __m256d current_a;
  __m256d current_b;

  double double_buffer[4] __attribute__((aligned(32)));

  double a_in_b, a_in_a, b_in_b;
  
  int i,j, i_plus_j;

  vector_length+=(4-vector_length%4);
    
  for(i=0;i<vector_length;i+=4) {
    current_a = _mm256_load_pd(a_d+i);
    current_b = _mm256_load_pd(b_d+i);
    a_in_b_vec = _mm256_fmadd_pd(current_a,current_b,a_in_b_vec);
    a_in_a_vec = _mm256_fmadd_pd(current_a,current_a,a_in_a_vec);
    b_in_b_vec = _mm256_fmadd_pd(current_b,current_b,b_in_b_vec);
  }

  _mm256_store_pd(double_buffer, a_in_b_vec);
  a_in_b = double_buffer[0]+double_buffer[1]+double_buffer[2]+double_buffer[3];

  _mm256_store_pd(double_buffer, a_in_a_vec);
  a_in_a = double_buffer[0]+double_buffer[1]+double_buffer[2]+double_buffer[3];

  _mm256_store_pd(double_buffer, b_in_b_vec);
  b_in_b = double_buffer[0]+double_buffer[1]+double_buffer[2]+double_buffer[3];
  
  return(1-(a_in_b/(sqrt(a_in_a)*sqrt(b_in_b))));
}

#else

double cosine_distance(char* a_in, char* b_in, size_t length) {
  size_t i;

  double* a=(double*)a_in;
  double* b=(double*)b_in;
  
  double sum_a = 0.0;
  double c_a = 0.0;
  double y_a,t_a;

  double sum_b = 0.0;
  double c_b = 0.0;
  double y_b,t_b;

  double sum_ab = 0.0;
  double c_ab = 0.0;
  double y_ab,t_ab;
  
  double a_value;
  double b_value;
  double a_times_b;
  double input_a, input_b, input_ab;

  for(i=0;i<length;i++) {
    a_value =  a[i];
    b_value =  b[i];
    
    input_ab = a_value*b_value;
    y_ab = input_ab - c_ab;
    t_ab = sum_ab + y_ab;
    c_ab = ( t_ab - sum_ab ) - y_ab;
    sum_ab = t_ab;

    input_a = a_value*a_value;
    y_a = input_a - c_a;
    t_a = sum_a + y_a;
    c_a = ( t_a - sum_a ) - y_a;
    sum_a = t_a;

    input_b = b_value*b_value;
    y_b = input_b - c_b;
    t_b = sum_b + y_b;
    c_b = ( t_b - sum_b ) - y_b;
    sum_b = t_b;
  }
  return(1-(sum_ab/(sqrt(sum_a)*sqrt(sum_b))));
}

#endif

void print_binary_embeddings(char* a, size_t vector_length) {

  size_t i;
  int v;
  printf("Binary embedding result: ");
  for(i=0;i<vector_length;i++) {
    v = get_value_in_binary_array_at_index(a,i);
    printf("%i",v);
  }
  printf("\n");
  
}
   

  
