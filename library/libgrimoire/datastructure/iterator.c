#include <libgrimoire/datastructure/iterator.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct priv_iterator priv_iterator_t;

struct priv_iterator {
	iterator_t public;

	int count;
	int index;

	void ** table;

	int cache_flag;
};

void * iterator_get_data(iterator_t * this, int index)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;

	if(priv->count > index)
		return priv->table[index];

	return NULL;
}

void iterator_set_data(iterator_t * this, void * data, int index)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;
	priv->table[index] = data;
}

int iterator_get_count(iterator_t * this)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;

	return priv->count;
}

void iterator_destroy(iterator_t * this)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;

	if(priv->cache_flag)
	{
		printf("%s(%d) Cache Iterator can't destroy.\n", __func__, __LINE__);
		return;
	}

	if(priv->table)
		free(priv->table);

	free(this);
}

void * iterator_next(iterator_t * this)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;
	void * ret;
	if(priv->index == priv->count)
		return NULL;

	ret = priv->table[priv->index];
	priv->index++;
	return ret;
}

void iterator_using_cache(iterator_t * this, int cache_flag)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;

	priv->cache_flag = cache_flag;
}

iterator_t * create_iterator(int count)
{
	priv_iterator_t * private;
	iterator_t * public;

	private = malloc(sizeof(priv_iterator_t));
	public = &private->public;
	public->destroy = iterator_destroy;
	public->get_count = iterator_get_count;
	public->get_data = iterator_get_data;
	public->set_data = iterator_set_data;
	public->next = iterator_next;
	public->using_cache = iterator_using_cache;

	private->count = count;
	private->index = 0;
	private->table = malloc(sizeof(void *) * count);

	return public;
}
