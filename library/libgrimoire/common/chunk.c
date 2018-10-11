#include <libgrimoire/common/chunk.h>

typedef struct priv_chunk priv_chunk_t;

struct priv_chunk {
	chunk_t public;

	int size;
	void * data;
};

int chunk_get_size(chunk_t * this)
{
	priv_chunk_t * priv = (priv_chunk_t *)this;
	return priv->size;
}

void * chunk_get_data(chunk_t * this)
{
	priv_chunk_t * priv = (priv_chunk_t *)this;
	return priv->data;
}

void chunk_destroy(chunk_t * this)
{
	priv_chunk_t * priv = (priv_chunk_t *)this;

	if(priv->data)
		free(priv->data);
	free(this);
}

chunk_t * create_chunk(int size, void * data)
{
	priv_chunk_t * private;
	chunk_t * public;

	private = malloc(sizeof(priv_chunk_t));
	public = &private->public;

	private->size = size;
	private->data = malloc(size);
	if(data)
		memcpy(private->data, data, size);

	public->get_data = chunk_get_data;
	public->get_size = chunk_get_size;
	public->destroy = chunk_destroy;

	return public;
}
