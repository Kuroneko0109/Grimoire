#ifndef _LIST_
#define _LIST_

#include <libgrimoire/datastructure/node.h>
#include <libgrimoire/datastructure/iterator.h>

typedef struct list list_t;

struct list {
	iterator_t * (*get_iterator)(list_t *);
	node_t * (*find)(list_t *, void *);
	void * (*find_data)(list_t *, void *);

	int (*count)(list_t *);

	node_t * (*enqueue_node)(list_t *, node_t *);
	node_t * (*enqueue_data)(list_t *, void *);
	node_t * (*dequeue_node)(list_t *);
	void * (*dequeue_data)(list_t *);

	node_t * (*detach)(list_t *, node_t *);

	void (*set_copy)(list_t *, void * (*)(void *));
	void (*set_sort)(list_t *, void (*)(iterator_t *, int (*)(void *, void *)));
	void (*sort)(list_t *);

	void (*foreach)(list_t *, void * (*)(void *));

	int (*lock)(list_t *);
	int (*unlock)(list_t *);

	void (*flush)(list_t *);
	void (*destroy)(list_t *);
	void (*dump)(list_t *);
};

list_t * create_list(
		void * (*method_destroyer)(void *),
		int (*method_compare)(void *, void *),
		void * (*method_dump)(void *));

#endif
