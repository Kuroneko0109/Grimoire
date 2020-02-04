#ifndef _LIST_
#define _LIST_

#include <libgrimoire/system/lock.h>
#include <libgrimoire/datastructure/node.h>
#include <libgrimoire/datastructure/iterator.h>

typedef struct list list_t;

struct list {
	iterator_t * (*get_iterator)(list_t *);
	iterator_t * (*create_iterator)(list_t *);
	node_t * (*find)(list_t *, int (*)(void *, void *), void *);
	void * (*find_data)(list_t *, int (*)(void *, void *), void *);

	int (*count)(list_t *);

	node_t * (*enqueue_node)(list_t *, node_t *);
	node_t * (*enqueue_data)(list_t *, void *);
	node_t * (*dequeue_node)(list_t *);
	void * (*dequeue_data)(list_t *);

	node_t * (*detach)(list_t *, node_t *);

	void (*set_copy)(list_t *, void * (*)(void *));
	void (*set_sort)(list_t *, void (*)(iterator_t *, int (*)(void *, void *)));
	void (*sort)(list_t *, int (*compare)(void *, void *));

	void (*foreach)(list_t *, void * (*)(void *));

	int (*lock)(list_t *);
	int (*unlock)(list_t *);

	void (*flush)(list_t *);
	void (*destroy)(list_t *);
	void (*dump)(list_t *);

	void (*iterator_sync)(list_t *);
	void (*using_iterator_cache)(list_t *, int);

	/* 판별 시, 각 리스트에 대해 모두 lock이 되어있어야 한다. */
	void (*set_likely)(list_t *, int (*)(list_t *, list_t *));
	int (*likely)(list_t *, list_t *);
};

list_t * create_list(
		int lock_mode,
		void * (*method_destroyer)(void *),
		void * (*method_dump)(void *));

#endif
