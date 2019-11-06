#include <libgrimoire/datastructure/state.h>
#include <stdio.h>

#include <stdlib.h>
#include <memory.h>

/*
 * Private
 */
typedef struct priv_state priv_state_t;
struct priv_state {
	state_t public;

	int current_state;

	/* Two-dimensional vector space */
	int vector_state;
	int vector_input;
	int transition_vector[0]; // vector_state * vector_input
};

/*
 * Transition
 */
int state_transition(state_t * this, int input)
{
	priv_state_t * priv = (priv_state_t *)this;
	int dimension_trans;
	int ret;

	dimension_trans =
		priv->current_state * priv->vector_input + // Two-Dimension
		input; // One-Dimension

	ret = priv->transition_vector[dimension_trans];
	if(0 <= ret)
		priv->current_state = ret;

	return ret;
}

int state_set_arc(state_t * this, int state,
		int input, int result)
{
	priv_state_t * priv = (priv_state_t *)this;
	int dimension_trans;

	dimension_trans =
		state * priv->vector_input + // Two-Dimension
		input; // One-Dimension

	priv->transition_vector[dimension_trans] = result;

	return result;
}

int state_set_state(state_t * this, int current_state)
{
	priv_state_t * priv = (priv_state_t *)this;
	priv->current_state = current_state;
	return priv->current_state;
}

int state_get_state(state_t * this)
{
	priv_state_t * priv = (priv_state_t *)this;
	return priv->current_state;
}

void state_dump(state_t * this)
{
	priv_state_t * priv = (priv_state_t *)this;
	int dimension_trans;
	int i;
	int j;

	printf(">>>> State dump <<<<\n");
	printf("now state : %d\n", priv->current_state);
	for(i=0;i<priv->vector_state;i++)
	{
		for(j=0;j<priv->vector_input;j++)
		{
			dimension_trans = i * priv->vector_state + j;
			if(priv->transition_vector[dimension_trans] < 0)
				continue;
			printf("when state %d, input %d => %d\n", i, j,
				priv->transition_vector[dimension_trans]);
		}
	}
	printf("<<<<        >>>>\n");
}

int state_is_final(state_t * this)
{
	priv_state_t * priv = (priv_state_t *)this;
	int current_dimension_entry;
	int ret = 0;
	int i;

	current_dimension_entry = priv->vector_input * priv->current_state;
	for(i=0;i<priv->vector_input;i++)
	{
		if(0 <= priv->transition_vector[current_dimension_entry+i])
		{
			ret = -1;
			break;
		}
	}

	return ret;
}

void state_destroy(state_t * this)
{
	free(this);
}

struct state * create_state(int vector_state, int vector_input)
{
	int alloc_size;
	priv_state_t * priv;
	state_t * public;
	int vector_size;
	int i;

	vector_size = vector_state * vector_input;
	alloc_size = sizeof(priv_state_t)
		+ sizeof(int) * vector_size;
	priv = malloc(alloc_size);
	memset(priv, 0, alloc_size);
	for(i=0;i<vector_size;i++)
	{
		priv->transition_vector[i] = -1;
	}

	public = &priv->public;

	/* init public method */
	public->transition = state_transition;
	public->set_arc = state_set_arc;
	public->set_state = state_set_state;
	public->get_state = state_get_state;
	public->dump = state_dump;
	public->is_final = state_is_final;
	public->destroy = state_destroy;

	priv->vector_state = vector_state;
	priv->vector_input = vector_input;
	priv->current_state = -1;

	return public;
}
