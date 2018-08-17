#include <libgrimoire/datastructure/trie_layer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

typedef struct priv_trie_layer priv_trie_layer_t;

struct priv_trie_layer {
	trie_layer_t public;

	void * data;
	int radix;
	int reference;
	trie_layer_t * parent;
	
	trie_layer_t * next[0];
};

trie_layer_t * trie_layer_get_parent(trie_layer_t * this)
{
	priv_trie_layer_t * priv = (priv_trie_layer_t *)this;
	return priv->parent;
}

trie_layer_t * trie_layer_get_next(trie_layer_t * this, int index)
{
	priv_trie_layer_t * priv = (priv_trie_layer_t *)this;
	return priv->next[index];
}

void trie_layer_set_next(trie_layer_t * this, trie_layer_t * next, int index)
{
	priv_trie_layer_t * priv = (priv_trie_layer_t *)this;
	priv->next[index] = next;
}

void * trie_layer_get_data(trie_layer_t * this)
{
	priv_trie_layer_t * priv = (priv_trie_layer_t *)this;
	return priv->data;
}

void trie_layer_dump(trie_layer_t * this)
{
	priv_trie_layer_t * priv = (priv_trie_layer_t *)this;
	trie_layer_t * layer;
	int i;

	printf("this layer : %p\n", this);
	for(i=0;i<priv->radix;i++)
	{
		layer = priv->next[i];
		if(layer)
		{
			printf("layer %d(%c) dump depth\n", i, (uint8_t)i);
			layer->dump(layer);
		}
	}
}

trie_layer_t * create_trie_layer(trie_layer_t * parent, int radix)
{
	priv_trie_layer_t * private;
	trie_layer_t * public;
	int i;

	private = malloc(offsetof(priv_trie_layer_t, next) + (sizeof(trie_layer_t *) * radix));
	public = &private->public;

	private->data = NULL;
	private->reference = 0;
	private->parent = parent;
	private->radix = radix;

	public->get_parent = trie_layer_get_parent;
	public->get_next = trie_layer_get_next;
	public->set_next = trie_layer_set_next;
	public->get_data = trie_layer_get_data;
	public->dump = trie_layer_dump;

	for(i=0;i<radix;i++)
		private->next[i] = NULL;

	return public;
}
