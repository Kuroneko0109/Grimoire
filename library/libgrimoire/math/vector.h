#ifndef _VECTOR_
#define _VECTOR_

#include <libgrimoire/datastructure/iterator.h>

typedef struct vector vector_t;
struct vector {
	void (*add_dimension)(vector_t * this, int size);
	void (*create_matrix)(vector_t * this);
	iterator_t * (*get_dimension)(vector_t * this);
	int (*get_dimension_count)(vector_t * this);
	int (*get_dimension_size)(vector_t * this, int dimension);

	void (*set_data)(vector_t * this, void * data);
};

vector_t * create_vector(void);

#endif
