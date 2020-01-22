#include <libgrimoire/datastructure/state2.h>
#include <stdio.h>

#include <stdlib.h>
#include <memory.h>

/*
 * Private
 */
typedef struct priv_state2 priv_state2_t;
struct priv_state2 {
	state2_t public;

	int current_state;

	size_t self_size;

	/* Two-dimensional vector space */
	int vector_state;
	int vector_input;

	int * transition_vector;	// vector_state2 * vector_input * sizeof(int)
	int * transition_output;	// same size of transition_vector;

	int hip_data[0];
};

/*
 * Transition
 */
int state2_transition(state2_t * this, int input)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	int ret = -1;
	int next_state;

	next_state = this->transition_check(this, input);
	if(0 <= next_state)
	{
		ret = priv->transition_output[priv->current_state * priv->vector_input + input];

		priv->current_state = next_state;
	}

	return ret;
}

int state2_transition_check(state2_t * this, int input)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	int dimension_trans;
	int ret;

	dimension_trans =
		priv->current_state * priv->vector_input + // Two-Dimension
		input; // One-Dimension

	ret = priv->transition_vector[dimension_trans];

	return ret;
}

int state2_set_arc(state2_t * this, int state,
		int input, int output, int result)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	int dimension_trans;

	dimension_trans =
		state * priv->vector_input + // Two-Dimension
		input; // One-Dimension

	priv->transition_vector[dimension_trans] = output;
	priv->transition_output[dimension_trans] = result;

	return output;
}

int state2_set_state(state2_t * this, int current_state)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	priv->current_state = current_state;
	return priv->current_state;
}

int state2_get_state(state2_t * this)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	return priv->current_state;
}

void state2_dump(state2_t * this)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	int dimension_trans;
	int i;
	int j;

	printf(">>>> State dump <<<<\n");
	printf("now state2 : %d\n", priv->current_state);
	for(i=0;i<priv->vector_state;i++)
	{
		for(j=0;j<priv->vector_input;j++)
		{
			dimension_trans = i * priv->vector_state + j;
			if(priv->transition_vector[dimension_trans] < 0)
				continue;
			printf("when state2 %d, input %d => %d\n", i, j,
				priv->transition_vector[dimension_trans]);
		}
	}
	printf("<<<<        >>>>\n");
}

int state2_is_final(state2_t * this)
{
	priv_state2_t * priv = (priv_state2_t *)this;
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

void state2_destroy(state2_t * this)
{
	free(this);
}

state2_t * state2_clone(state2_t * this)
{
	priv_state2_t * priv = (priv_state2_t *)this;
	state2_t * child = malloc(priv->self_size);

	memcpy(child, priv, priv->self_size);

	return child;
}

struct state2 * create_state2(int vector_state, int vector_input)
{
	int alloc_size;
	priv_state2_t * priv;
	state2_t * public;
	int vector_size;
	int i;

	vector_size = vector_state * vector_input;
	alloc_size = sizeof(priv_state2_t)
		+ sizeof(int) * vector_size * 2;	// transition vector + output table
	priv = malloc(alloc_size);
	memset(priv, 0, alloc_size);
	priv->transition_vector = priv->hip_data;
	priv->transition_output = priv->transition_vector + vector_size;
	for(i=0;i<vector_size;i++)
	{
		priv->transition_vector[i] = -1;
		priv->transition_output[i] = -1;
	}

	public = &priv->public;

	priv->self_size = alloc_size;

	/* init public method */
	public->transition = state2_transition;
	public->set_arc = state2_set_arc;
	public->set_state = state2_set_state;
	public->get_state = state2_get_state;
	public->dump = state2_dump;
	public->is_final = state2_is_final;
	public->clone = state2_clone;
	public->destroy = state2_destroy;

	priv->vector_state = vector_state;
	priv->vector_input = vector_input;
	priv->current_state = -1;

	return public;
}
