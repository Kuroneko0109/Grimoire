#ifndef _STATE_
#define _STATE_

/*
 * Public
 */
typedef struct state state_t;
struct state {
	/* do transition */
	int (*transition)(state_t * this, int input);

	/* 
	 * set transition arc
	 * state(input)=>result
	 */
	int (*set_arc)(state_t * this,
			int state, int input, int result);

	/* set state */
	int (*set_state)(state_t * this, int current_state);

	/* return current state */
	int (*get_state)(state_t * this);

	/* dump arc map */
	void (*dump)(state_t * this);

	/* temp callback */
	void (*set_state_callback)(state_t * this,
			void (*cb)(int));

	int (*is_final)(state_t * this);

	/* destroy */
	void (*destroy)(state_t * this);
	
};

/* count of state, count of case of input */
state_t * create_state(int vector_state, int vector_input);

#endif
