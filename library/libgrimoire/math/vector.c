#include <libgrimoire/math/vector.h>
#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/datastructure/iterator.h>
#include <stdlib.h>
#include <string.h>

typedef struct _vector priv_vector_t;
struct _vector {
	vector_t public;

	list_t * dimensions;
	int dimension_count;
	double * dimension_root;
};

void vector_add_dimension(vector_t * this, int size)
{
	priv_vector_t * private = (priv_vector_t *)this;
	int * data;

	data = malloc(sizeof(int));
	*data = size;
	private->dimension_count++;
	private->dimensions->enqueue_data(private->dimensions, data);
}

void vector_create_matrix(vector_t * this)
{
	priv_vector_t * private = (priv_vector_t *)this;
	iterator_t * iterator;
	int matrix_size = 1;
	int * value;

	if(private->dimension_root)
		free(private->dimension_root);

	iterator = private->dimensions->get_iterator(private->dimensions);

	while((value = iterator->next(iterator)))
		matrix_size *= *value;

	iterator->destroy(iterator);

	private->dimension_root = malloc(matrix_size * sizeof(double));
}

iterator_t * vector_get_dimension(vector_t * this)
{
	priv_vector_t * private = (priv_vector_t *)this;
	return private->dimensions->get_iterator(private->dimensions);
}

int vector_get_dimension_size(vector_t * this, int dimension)
{
	priv_vector_t * private = (priv_vector_t *)this;
	iterator_t * iterator = private->dimensions->get_iterator(private->dimensions);
	int ret;

	ret = *(int *)iterator->get_data(iterator, dimension);
	iterator->destroy(iterator);

	return ret;
}

int vector_get_dimension_count(vector_t * this)
{
	priv_vector_t * private = (priv_vector_t *)this;
	return private->dimension_count;
}

int vector_set_data(vector_t * this, void * data)
{
	return 0;
}

vector_t * create_vector(void)
{
	priv_vector_t * private = malloc(sizeof(priv_vector_t));
	vector_t * public = &private->public;

	public->add_dimension = vector_add_dimension;
	public->create_matrix = vector_create_matrix;
	public->get_dimension = vector_get_dimension;
	public->get_dimension_count = vector_get_dimension_count;
	public->get_dimension_size = vector_get_dimension_size;

	private->dimensions = create_list(LOCK_MUTEX, NULL, NULL);

	private->dimension_count = 0;
	private->dimension_root = NULL;

	return public;
}
