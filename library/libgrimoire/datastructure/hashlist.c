#include <stdlib.h>
#include <unistd.h>
#include <libgrimoire/datastructure/hashlist.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_hashlist priv_hashlist_t;

struct priv_hashlist {
	hashlist_t public;

	int (*hasher)(hashlist_t * this, void * data);
	int (*_hasher)(void * data);
	int hash_size;

	list_t * chain[0];
};

/*
struct hasher_extend {
	uint32_t key;
	void * data;
};
*/

int hashlist_hasher_wrap(hashlist_t * this, void * data)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int ret;
	ret = priv->_hasher(data) % priv->hash_size;

	if(ret<0)
		ret *= -1;

	return ret;
}

void hashlist_input_data(hashlist_t * this, void * data)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int index = priv->hasher(this, data);
	list_t * chain = priv->chain[index];

	chain->enqueue_data(chain, data);
}

void * hashlist_find_data(hashlist_t * this, void * sample)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int index = priv->hasher(this, sample);
	list_t * chain = priv->chain[index];

	return chain->find_data(chain, sample);
}

hashlist_t * create_hashlist(
		int (*hasher)(void *),
		int hash_size,
		void * (*method_destroyer)(void *),
		int (*method_compare)(void *, void *),
		void * (*method_dump)(void *))
{
	priv_hashlist_t * private;
	hashlist_t * public;
	int i;

	private = malloc(sizeof(priv_hashlist_t) + sizeof(list_t *) * hash_size);
	public = &private->public;

	private->hasher = hashlist_hasher_wrap;
	private->_hasher = hasher;
	private->hash_size = hash_size;

	public->input_data = hashlist_input_data;
	public->find_data = hashlist_find_data;

	for(i=0;i<hash_size;i++)
		private->chain[i] = create_list(method_destroyer, method_compare, method_dump);

	return public;
}
