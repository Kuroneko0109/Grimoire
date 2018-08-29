#include <libgrimoire/datastructure/trie.h>
#include <libgrimoire/datastructure/trie_layer.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef struct priv_trie priv_trie_t;

struct priv_trie {
	trie_t public;

	uint8_t mapping_table[256];
	int radix;

	trie_layer_t * root;
};

int trie_insert(trie_t * this, void * src, int src_len, void * data)
{
	priv_trie_t * priv = (priv_trie_t *)this;
	uint8_t * byte = src;
	trie_layer_t * layer;
	trie_layer_t * next_layer;
	int i;

	this->dump(this);

	layer = priv->root;
	for(i=0;i<src_len;i++)
	{
		next_layer = layer->get_next(layer, priv->mapping_table[byte[i]]);
		if(NULL == next_layer)
		{
			next_layer = create_trie_layer(layer, priv->radix);
			layer->set_next(layer, next_layer, priv->mapping_table[byte[i]]);
		}

		layer = next_layer;
	}

	layer->set_data(layer, data);

	this->dump(this);

	return 0;
}

int trie_remove(trie_t * this, void * src, int src_len)
{
	priv_trie_t * priv = (priv_trie_t *)this;
	uint8_t * byte = src;
	int i;

	return 0;
}

void * trie_find(trie_t * this, void * src, int src_len)
{
	priv_trie_t * priv = (priv_trie_t *)this;
	uint8_t * byte = src;
	int i;

	return NULL;
}

void trie_dump(trie_t * this)
{
	priv_trie_t * priv = (priv_trie_t *)this;
	trie_layer_t * layer = priv->root;
	int i;

	printf("%s\n", __func__);
	layer->dump(layer);

	return 0;
}

trie_t * create_trie(int radix, uint8_t * mapping_table)
{
	priv_trie_t * private;
	trie_t * public;
	int i;

	private = malloc(sizeof(priv_trie_t));
	public = &private->public;

	private->radix = radix;
	private->root = create_trie_layer(NULL, private->radix);
	if(mapping_table)
		memcpy(private->mapping_table, mapping_table, sizeof(private->mapping_table));
	else
	{
		for(i=0;i<sizeof(private->mapping_table);i++)
			private->mapping_table[i] = i;
	}

	public->insert = trie_insert;
	public->remove = trie_remove;
	public->find = trie_find;
	public->dump = trie_dump;

	return public;
}
