#ifndef _TRIE_
#define _TRIE_

#include <stdint.h>
#include <libgrimoire/datastructure/trie_layer.h>

typedef struct trie trie_t;

struct trie {
	int (*insert)(trie_t * this, void * src, int src_len);
	int (*remove)(trie_t * this, void * src, int src_len);
	trie_layer_t * (*find)(trie_t * this, void * src, int src_len);
	void (*dump)(trie_t * this);
};

/* search function returns next layer index */
trie_t * create_trie(int radix, uint8_t * mapping_table);

#endif
