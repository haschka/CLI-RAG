/*! \file binary_array.h
 *  \brief defines function for binary array manipulation
 */ 

/*! \brief Allocates a binary array and sets all values to zero. */
char* alloc_and_set_zero_binary_array(size_t array_length);

/*! \brief Sets all values in a binary array to zero. */
char* set_zero_binary_array(char* b_array,size_t array_length);

/*! \brief Allocates a binary array. */
char* alloc_binary_array(size_t array_length);

/*! \brief Results a binary array from the OR result of two binary array. 
 *  Calculates the OR array from the arrays a and b. All arrays have to be of 
 *  the same length and have to be allocated prior calling this function
 */
void binary_array_or(char* result, size_t array_length, char* a, char* b);

/*! \brief Results a binary array from the AND result of two binary array. 
 *  Calculates the OR array from the arrays a and b. All arrays have to be of 
 *  the same length and have to be allocated prior calling this function
 */
void binary_array_and(char* result, size_t array_length, char* a, char* b);

/*! \brief Sets the value in a binary array to 1 at index. */
void set_value_in_binary_array_at_index(char* b_array,
					size_t index);

/*! \brief Gets the value in a binary array at index. 
 *  This function obtains the value of a binary array at a specified index and 
 *  returns it as integer 0 or 1 */ 
int get_value_in_binary_array_at_index(char* b_array,
				       size_t index);
