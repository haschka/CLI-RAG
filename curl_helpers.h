size_t write_function_callback(char* in_data,
			       size_t size,
			       size_t nmemb,
			       void* clientdata);

size_t write_function_callback_stream_llm(char* in_data,
					  size_t size,
					  size_t nmemb,
					  void* clientdata);

typedef struct {
  char* data;
  size_t size;
} response_data;
