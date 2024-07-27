char** texts_from_files(char** files, int n_files);
char** cut_texts_in_subtexts(char* text,
			     size_t text_length,
			     size_t requested_length,
			     size_t* n_texts);
