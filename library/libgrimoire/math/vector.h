#ifndef _VECTOR_
#define _VECTOR_

typedef struct vector vector_t;
struct vector {
	void (*create_dimension)(vector_t * this, int size);
	int (*get_dimension)(vector_t * this);
	int (*get_size)(vector_t * this, int dimension);
};

vector_t * create_vector(const char * vector_type);

#endif
