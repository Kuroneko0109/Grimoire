#ifndef _HASHLIST_
#define _HASHLIST_

#include <libgrimoire/datastructure/list.h>

typedef struct hashlist hashlist_t;

struct hashlist {
	void (*input_data)(hashlist_t * this, void * data);
	void * (*find_data)(hashlist_t * this, void * data);
};

hashlist_t * create_hashlist(
		int (*hasher)(void *),
		int hash_size,
		void * (*method_destroyer)(void *),
		int (*method_compare)(void *, void *),
		void * (*method_dump)(void *));

#endif
