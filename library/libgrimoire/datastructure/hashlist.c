#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgrimoire/system/memory.h>
#include <libgrimoire/datastructure/hashlist.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_hashlist priv_hashlist_t;

struct priv_hashlist {
	hashlist_t public;

	unsigned int (*hasher)(hashlist_t * this, void * data);
	unsigned int (*_hasher)(void * data);
	unsigned int hash_size;

	list_t * chain[0];

	void * (*method_destroyer)(void *);
	void * (*method_dump)(void *);
};

/*
struct hasher_extend {
	uint32_t key;
	void * data;
};
*/

unsigned int hashlist_hasher_wrap(hashlist_t * this, void * data)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	unsigned int ret;
	ret = priv->_hasher(data) % priv->hash_size;

	return ret;
}

void hashlist_input_data(hashlist_t * this, void * data)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int index = priv->hasher(this, data);
	list_t * chain = priv->chain[index];

	chain->enqueue_data(chain, data);
}

void * hashlist_find_data(hashlist_t * this, int (*compare)(void *, void *), void * sample)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int index = priv->hasher(this, sample);
	list_t * chain = priv->chain[index];

	return chain->find_data(chain, compare, sample);
}

void hashlist_dump(hashlist_t * this)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	unsigned int i;

	for(i=0;i<priv->hash_size;i++)
		priv->chain[i]->dump(priv->chain[i]);
}

void hashlist_count(hashlist_t * this)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	unsigned int i;
	int count;

	count = 0;
	for(i=0;i<priv->hash_size;i++)
		count += priv->chain[i]->count(priv->chain[i]);

	printf("hashlist count : %d\n", count);
}

list_t * hashlist_merge(hashlist_t * this)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	list_t * ret;
	list_t * chain;
	iterator_t * iterator;
	unsigned int i;
	void * data;

	ret = create_list(LOCK_MUTEX, priv->method_destroyer, priv->method_dump);
	for(i=0;i<priv->hash_size;i++)
	{
		chain = priv->chain[i];
		iterator = chain->get_iterator(chain);

		while((data = iterator->next(iterator)))
			ret->enqueue_data(ret, data);

		iterator->destroy(iterator);
	}

	printf("Complete merging. %d\n", ret->count(ret));

	return ret;
}

void hashlist_set_copy(hashlist_t * this, void * (*method_copy)(void *))
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	unsigned int i;

	for(i=0;i<priv->hash_size;i++)
		priv->chain[i]->set_copy(priv->chain[i], method_copy);
}

hashlist_t * create_hashlist(
		unsigned int (*hasher)(void *),
		unsigned int hash_size,
		void * (*method_destroyer)(void *),
		void * (*method_dump)(void *))
{
	priv_hashlist_t * private;
	hashlist_t * public;
	unsigned int i;

	private = galloc(sizeof(priv_hashlist_t) + sizeof(list_t *) * hash_size);
	public = &private->public;

	private->hasher = hashlist_hasher_wrap;
	private->_hasher = hasher;
	private->hash_size = hash_size;
	private->method_destroyer = method_destroyer;
	private->method_dump = method_dump;

	public->input_data = hashlist_input_data;
	public->find_data = hashlist_find_data;
	public->set_copy = hashlist_set_copy;
	public->dump = hashlist_dump;
	public->count = hashlist_count;
	public->merge = hashlist_merge;

	for(i=0;i<hash_size;i++)
		private->chain[i] = create_list(LOCK_MUTEX, method_destroyer, method_dump);

	return public;
}
