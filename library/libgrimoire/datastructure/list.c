#include <stdio.h>
#include <stdlib.h>

#include <libgrimoire/system/memory.h>
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
	void (*method_sort)(iterator_t *, int (*)(void *, void *));
	void * (*method_copy)(void *);
	void * (*method_dump)(void *);
	void * (*method_merge)(void *);

	int (*method_likely)(list_t *, list_t *);

	lock_t * lock;

	int using_iterator_cache;
	iterator_t * cache;
};

iterator_t * list_get_iterator(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	if(priv->using_iterator_cache)
	{
		if(NULL == priv->cache)
			this->iterator_sync(this);

		return priv->cache;
	}

	return this->create_iterator(this);
}

iterator_t * list_create_iterator(list_t * this)
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

node_t * list_find(list_t * this, int (*compare)(void *, void *), void * sample)
{
	priv_list_t * priv = (priv_list_t *)this;
	node_t * node;

	for(node=priv->head;node;node=node->get_rear(node))
	{
		if(0 == compare(node->get_data(node), sample))
			break;
	}

	return node;
}

void * list_find_data(list_t * this, int (*compare)(void *, void *), void * sample)
{
	node_t * node;
	node = this->find(this, compare, sample);

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

void list_iterator_sync(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;

	if(priv->using_iterator_cache)
	{
		if(NULL != priv->cache)
		{
			/* iterator에 캐시플래그 없애줘야 디스트로이할때 진짜 없어짐. */
			priv->cache->using_cache(priv->cache, 0);
			priv->cache->destroy(priv->cache);

			/* create한다음엔 다시 캐시플래그 세워줌. 외부에서 실수로 Destroy해도 안사라짐. */
			priv->cache = this->create_iterator(this);
			priv->cache->using_cache(priv->cache, 1);
		}
		else
		{
			priv->cache = this->create_iterator(this);
			priv->cache->using_cache(priv->cache, 1);
		}
	}
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

	this->iterator_sync(this);

	return node;
}

void * list_dequeue_data(list_t * this)
{
	node_t * node;
	void * data = NULL;

	node = this->dequeue_node(this);
	if(node)
	{
		data = node->get_data(node);
		node->set_data(node, NULL);
		node->destroy(node);
	}

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

	this->iterator_sync(this);

	return node;
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
	priv_list_t * priv = (priv_list_t *)this;
	this->flush(this);

	priv->lock->destroy(priv->lock);

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

	priv->head = NULL;
	priv->tail = NULL;
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

void list_sort(list_t * this, int (*compare)(void *, void *))
{
	priv_list_t * priv = (priv_list_t *)this;
	iterator_t * iterator;
	node_t * node;

	void * data;

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
		priv->method_sort(iterator, compare);

	while(this->dequeue_data(this))
		;
	while(iterator->next(iterator))
		;
	while((data = iterator->next(iterator)))
		this->enqueue_data(this, data);

	iterator->destroy(iterator);
}

void list_using_iterator_cache(list_t * this, int using)
{
	priv_list_t * priv = (priv_list_t *)this;

	priv->using_iterator_cache = using;
}

void list_set_likely(list_t * this, int (*method_likely)(list_t *, list_t *))
{
	priv_list_t * priv = (priv_list_t *)this;

	priv->method_likely = method_likely;
}

int list_likely(list_t * this, list_t * list_obj)
{
	priv_list_t * priv = (priv_list_t *)this;
	int ret = -1;

	if(priv->method_likely)
	{
		ret = priv->method_likely(this, list_obj);
	}

	return ret;
}

void * list_method_default_merge(list_t * this)
{
	iterator_t * iter = this->get_iterator(this);
	void * tmp;
	void * ret;
	uint8_t * ptr;
	size_t size = 0;

	while((tmp = iter->next(iter)))
		size += _msize(tmp);

	if(0 == size)
		goto end;

	ret = malloc(size);
	if(NULL == ret)
		goto end;

	ptr = ret;
	while((tmp = iter->next(iter)))
	{
		memcpy(ptr, tmp, _msize(tmp));
		ptr += _msize(tmp);
	}

end :
	iter->destroy(iter);
	return ret;
}

void * list_merge(list_t * this)
{
	priv_list_t * priv = (priv_list_t *)this;
	return priv->method_merge(this);
}

list_t * create_list(
		int lock_mode,
		void * (*method_destroyer)(void *),
		void * (*method_dump)(void *))
{
	priv_list_t * private;
	list_t * public;

	private = galloc(sizeof(priv_list_t));
	public = &private->public;

	private->head = NULL;
	private->tail = NULL;

	private->method_destroyer = method_destroyer;
	private->method_copy = NULL;
	private->method_sort = NULL;
	private->method_merge = list_method_default_merge;
	private->method_dump = method_dump;
	private->lock = create_lock(lock_mode);
	private->cache = NULL;
	private->using_iterator_cache = 0;

	public->iterator_sync = list_iterator_sync;
	public->set_copy = list_set_copy;
	public->count = list_count;
	public->get_iterator = list_get_iterator;
	public->create_iterator = list_create_iterator;
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
	public->merge = list_merge;
	public->foreach = list_foreach;
	public->dump = list_dump;
	public->flush = list_flush;
	public->destroy = list_destroy;
	public->using_iterator_cache = list_using_iterator_cache;
	public->set_likely = list_set_likely;
	public->likely = list_likely;

	return public;
}
