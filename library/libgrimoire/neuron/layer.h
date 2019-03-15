#ifndef _LAYER_
#define _LAYER_

typedef struct layer layer_t;
struct layer {
	void (*input)(layer_t * this);

	void (*import)(layer_t * this, layer_t * l);
	
	int (*get_dimension)(layer_t * this);
};

layer_t * create_layer(void);

#endif
