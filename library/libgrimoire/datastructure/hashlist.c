#include <libgrimoire/datastructure/hashlist.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_hashlist priv_hashlist_t;

struct priv_hashlist {
	hashlist_t public;

	int (*hasher)(void * data);
	int hash_size;

	list_t * list[0];
};

/*
struct hasher_extend {
	uint32_t key;
	void * data;
};
*/

void hashlist_input_data(hashlist_t * this, void * data)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int index = priv->hasher(data);
	list_t * list = priv->list[index];

	list->enqueue_data(list, data);
}

void * hashlist_find_data(hashlist_t * this, void * sample)
{
	priv_hashlist_t * priv = (priv_hashlist_t *)this;
	int index = priv->hasher(sample);
	list_t * list = priv->list[index];

	return list->find_data(list, sample);
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

	private->hasher = hasher;
	private->hash_size = hash_size;

	public->input_data = hashlist_input_data;
	public->find_data = hashlist_find_data;

	for(i=0;i<hash_size;i++)
		private->list[i] = create_list(method_destroyer, method_compare, method_dump);

	return public;
}
