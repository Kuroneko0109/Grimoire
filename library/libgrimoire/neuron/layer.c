#include <libgrimoire/neuron/layer.h>
#include <libgrimoire/datastructure/list.h>
#include <stdlib.h>

typedef struct _layer priv_layer_t;
struct _layer {
	layer_t public;

	layer_t * parent;
	list_t * list;
};

layer_t * create_layer(void)
{
	priv_layer_t * private = malloc(sizeof(priv_layer_t));
	layer_t * public = &private->public;

	public->input = layer_input;
	public->import = layer_import;

	private->parent = NULL;

	return public;
}
