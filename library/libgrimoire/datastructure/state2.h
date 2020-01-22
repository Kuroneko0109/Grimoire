#ifndef _STATE2_
#define _STATE2_

/*
 * Public
 */
typedef struct state2 state2_t;
struct state2 {
	/* do transition. return output. if you need current state, then use get_state() */
	int (*transition)(state2_t * this, int input);
	int (*transition_check)(state2_t * this, int input);

	/* 
	 * set transition arc
	 * state(input)=>next_state(result)
	 * state(input)=>next_state(result).output?output:-1;
	 */
	int (*set_arc)(state2_t * this,
			int state2, int input, int result, int output);

	/* set state */
	int (*set_state)(state2_t * this, int current_state);

	/* return current state */
	int (*get_state)(state2_t * this);

	/* dump arc map */
	void (*dump)(state2_t * this);

	int (*is_final)(state2_t * this);

	state2_t * (*clone)(state2_t * this);

	/* destroy */
	void (*destroy)(state2_t * this);
};

/* count of state2, count of case of input */
state2_t * create_state2(int vector_state, int vector_input);

#endif
