#include <libgrimoire/math/vector.h>
#include <libgrimoire/datastructure/list.h>
#include <stdlib.h>

typedef struct _vector priv_vector_t;
struct _vector {
	vector_t public;

	list_t * dimensions;
};

vector_t * create_vector(const char * vector_type)
{
	priv_vector_t * private = malloc(sizeof(priv_vector_t));
	vector_t * public = &private->public;

	public->create_dimension = vector_create_dimension;
	public->get_dimension = vector_get_dimension;
	public->get_size = vector_get_size;

	dimensions = create_list(NULL, NULL, NULL);

	return public;
}
