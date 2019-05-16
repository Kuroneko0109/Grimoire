#include <stdio.h>
#include <stdlib.h>

#include <libgrimoire/common/common.h>
#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/datastructure/iterator.h>
#include <libgrimoire/datastructure/node.h>
#include <libgrimoire/system/lock.h>

typedef struct priv_list priv_list_t;

struct priv_list {
	list_t public;

	node_t * head;
	node_t * tail;

	void * (*method_destroyer)(void *);
	int (*method_compare)(void *, void *);
	void (*method_sort)(iterator_t *, int (*)(void *, void *));
	void * (*method_copy)(void *);
	void * (*method_dump)(void *);

	lock_t * lock;
};

iterator_t * list_get_iterator(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	iterator_t * iterator;
	node_t * node;
	void * (*method_copy)(void *) = priv->method_copy;
	void * data;

	int count;
	int i;

	count = this->count(this);
	iterator = create_iterator(count);

	node = priv->head;
	for(i=0;i<count;i++)
	{
		if(method_copy)
			data = method_copy(node->get_data(node));
		else
			data = node->get_data(node);

		iterator->set_data(iterator, data, i);
		node = node->get_rear(node);
	}

	return iterator;
}

node_t * list_find(list_t * this, void * sample)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node;

	if(NULL == priv->method_compare)
		return NULL;

	for(node=priv->head;node;node=node->get_rear(node))
	{
		if(0 == priv->method_compare(node->get_data(node), sample))
			break;
	}

	return node;
}

void * list_find_data(list_t * this, void * sample)
{
	node_t * node;
	node = this->find(this, sample);

	if(node)
		return node->get_data(node);

	return NULL;
}

node_t * list_detach(list_t * this, node_t * node)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * front;
	node_t * rear;

	if(node == NULL)
	{
		printf("NULL node\n");
		return NULL;
	}

	front = node->get_front(node);
	rear = node->get_rear(node);

	if(front)
		front->set_rear(front, rear);
	else
		priv->head = rear;

	if(rear)
		rear->set_front(rear, front);
	else
		priv->tail = front;

	return node;
}

node_t * list_enqueue_data(list_t * this, void * data)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node;

	node = create_node(data, priv->method_destroyer);
	node->set_front(node, NULL);
	node->set_rear(node, NULL);
	return this->enqueue_node(this, node);
}

node_t * list_enqueue_node(list_t * this, node_t * node)
{
	priv_list_t * priv = (priv_list_t *)this;

	if(priv->tail)
	{
		priv->tail->set_rear(priv->tail, node);
		node->set_front(node, priv->tail);
		priv->tail = node;
		node->set_rear(node, NULL);
	}
	else	// empty
	{
		priv->head = node;
		priv->tail = node;
	}

	return node;
}

void * list_dequeue_data(list_t * this)
{
	node_t * node;
	void * data;

	node = this->dequeue_node(this);
	data = node->get_data(node);
	node->set_data(node, NULL);
	node->destroy(node);

	return data;
}

node_t * list_dequeue_node(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node = priv->head;

	if(!node)
		return NULL;

	priv->head = node->get_rear(node);
	if(!priv->head)
		priv->tail = NULL;

	return NULL;
}

int list_lock(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	priv->lock->lock(priv->lock);
	return 0;
}

int list_unlock(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	priv->lock->unlock(priv->lock);
	return 0;
}

void list_foreach(list_t * this, void * (*func)(void *))
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node;
 
	for(node=priv->head;node;node=node->get_rear(node))
		func(node->get_data(node));
}

void list_dump(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;

	printf("list dump\n");
	printf("head : %p, tail : %p\n", priv->head, priv->tail);
//	for(node=priv->head;node;node=node->get_rear(node))
//		printf("node(%d, %p) front : %p, rear : %p\n", i++, node, node->get_front(node), node->get_rear(node));

	if(priv->method_dump)
		this->foreach(this, priv->method_dump);
}

void list_destroy(list_t * this)
{
	this->flush(this);

	free(this);
}

void list_flush(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node;
	node_t * tmp;

	node = priv->head;
	while(node)
	{
		tmp = node;
		node = node->get_rear(node);
		tmp->destroy(tmp);
	}
}

int list_count(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node;
	int i = 0;

	for(node=priv->head;node;node=node->get_rear(node))
		i++;

	return i;
}

void list_set_copy(list_t * this, void * (*method_copy)(void *))
{
	priv_list_t * priv = (priv_list_t *)this;
	priv->method_copy = method_copy;
}

void list_set_sort(list_t * this, void (*method_sort)(iterator_t *, int (*)(void *, void *)))
{
	priv_list_t * priv = (priv_list_t *)this;

	priv->method_sort = method_sort;
}

void list_sort(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	iterator_t * iterator;
	node_t * node;

	int count;
	int i;

	count = this->count(this);
	iterator = create_iterator(count);

	node = priv->head;
	for(i=0;i<count;i++)
	{
		iterator->set_data(iterator, node, i);
		node = node->get_rear(node);
	}

	if(priv->method_sort)
		priv->method_sort(iterator, priv->method_compare);

	iterator->destroy(iterator);
}

list_t * create_list(
		void * (*method_destroyer)(void *),
		int (*method_compare)(void *, void *),
		void * (*method_dump)(void *))
{
	priv_list_t * private;
	list_t * public;

	private = malloc(sizeof(priv_list_t));
	public = &private->public;

	private->head = NULL;
	private->tail = NULL;

	private->method_destroyer = method_destroyer;
	private->method_compare = method_compare;
	private->method_copy = NULL;
	private->method_sort = NULL;
	private->method_dump = method_dump;
	private->lock = create_lock();

	public->set_copy = list_set_copy;
	public->count = list_count;
	public->get_iterator = list_get_iterator;
	public->find = list_find;
	public->find_data = list_find_data;
	public->enqueue_data = list_enqueue_data;
	public->enqueue_node = list_enqueue_node;
	public->dequeue_data = list_dequeue_data;
	public->dequeue_node = list_dequeue_node;
	public->detach = list_detach;
	public->lock = list_lock;
	public->unlock = list_unlock;
	public->set_sort = list_set_sort;
	public->sort = list_sort;
	public->foreach = list_foreach;
	public->dump = list_dump;
	public->flush = list_flush;
	public->destroy = list_destroy;

	return public;
}
