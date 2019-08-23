#ifndef _NODE_
#define _NODE_

#define FRONT	(0)
#define REAR	(1)

typedef struct node node_t;

struct node {
	void * (*get_data)(node_t * this);
	void (*set_data)(node_t * this, void * data);
	node_t * (*get_front)(node_t * this);
	node_t * (*set_front)(node_t * this, node_t * node);
	node_t * (*get_rear)(node_t * this);
	node_t * (*set_rear)(node_t * this, node_t * node);
	void (*destroy)(node_t * this);
};

node_t * create_node(void * data, void * (*destroyer)(void *));

#endif
