#include <libgrimoire/datastructure/iterator.h>
#include <stdlib.h>

typedef struct priv_iterator priv_iterator_t;

struct priv_iterator {
	iterator_t public;

	int count;
	int index;

	void ** table;
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

	if(priv->table)
		free(priv->table);

	free(this);
}

void * iterator_next(iterator_t * this)
{
	priv_iterator_t * priv = (priv_iterator_t *)this;
	void * ret;
	int i;
	if(priv->index == priv->count)
		return NULL;

	ret = priv->table[priv->index];
	priv->index++;
	return ret;
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

	private->count = count;
	private->index = 0;
	private->table = malloc(sizeof(void *) * count);

	return public;
}
