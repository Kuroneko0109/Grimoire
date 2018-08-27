#ifndef _TRIE_LAYER_
#define _TRIE_LAYER_

typedef struct trie_layer trie_layer_t;

struct trie_layer {
	trie_layer_t * (*get_parent)(trie_layer_t * this);
	trie_layer_t * (*get_next)(trie_layer_t * this, int index);
	void (*set_next)(trie_layer_t * this, trie_layer_t * next, int index);
	void * (*get_data)(trie_layer_t * this);
	void (*set_data)(trie_layer_t * this, void * data);
	void (*dump)(trie_layer_t * this);
};

trie_layer_t * create_trie_layer(trie_layer_t * parent, int radix);

#endif
