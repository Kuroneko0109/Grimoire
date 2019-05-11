#ifndef _HASHLIST_
#define _HASHLIST_

#include <libgrimoire/datastructure/list.h>

typedef struct hashlist hashlist_t;

struct hashlist {
	void (*input_data)(hashlist_t * this, void * data);
	void * (*find_data)(hashlist_t * this, void * data);
	void (*set_copy)(hashlist_t * this, void * (*method_copy)(void *));
	void (*dump)(hashlist_t * this);
	void (*count)(hashlist_t * this);
	list_t * (*merge)(hashlist_t * this);
};

hashlist_t * create_hashlist(
		unsigned int (*hasher)(void *),
		unsigned int hash_size,
		void * (*method_destroyer)(void *),
		int (*method_compare)(void *, void *),
		void * (*method_dump)(void *));

#endif
