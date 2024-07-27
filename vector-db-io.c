#include"dataset.h"
#include"vector-db.h"

void dataset_to_v_database(dataset* ds, database* db, char** texts){

  size_t vector_size;
#if defined (_COS_DISTANCE)
  vector_size = sizeof(double)*db->n_dimesions;
#elif defined (_YULE_DISTANCE)
  vector_size = (db->n_dimensions)/8+1;
#endif
  
  db->n_entries = ds->n_values;
  db->vector_length = ds->n_dimensions;

  db->vector = (char**)malloc(sizeof(char*)*db->n_entries);

  for (i=0;i<db->n_entries;i++) {
    posix_memalign(db->vector+i,
		   32,
		   vector_size+(32 - (vector_size % 32)));
    memcpy(db->vector[i],ds->values[i],vector_size);
    for (j=0;j<(32-vector_size % 32);j++) {
      db->vector[i][vector_size+j] = 0;
    }
    free(ds->values[i])
  }
  free(ds->values);
  ds->values = db->vector;
  db->text = texts;

#if defined (_COS_DISTANCE)
  db->type = DOUBLE_VECTOR;
#elif defined (_YULE_DISTANCE)
  db->type = BINARY_VECTOR;
}


