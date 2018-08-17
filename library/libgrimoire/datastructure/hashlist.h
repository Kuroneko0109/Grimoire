#ifndef _HASHLIST_
#define _HASHLIST_

#include <libgrimoire/datastructure/list.h>

typedef struct hashlist hashlist_t;

struct hashlist {
	int (*input_data)(hashlist_t * this, void * data);
};

hashlist_t * create_hashlist(int (*hasher)(void *), int size);

#endif
