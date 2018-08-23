#ifndef _ITERATOR_
#define _ITERATOR_

typedef struct iterator iterator_t;

struct iterator {
	void (*destroy)(iterator_t * this);
	int (*get_count)(iterator_t * this);
	void * (*get_data)(iterator_t * this, int index);
	void (*set_data)(iterator_t * this, void * data, int index);
	void * (*next)(iterator_t * this);
};

iterator_t * create_iterator(int count);

#endif
