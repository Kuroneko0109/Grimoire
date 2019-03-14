#ifndef _STATE_
#define _STATE_

typedef struct state state_t;
struct state {
	void (*input)(state_t *, void *);
};

state_t * create_state(void);

#endif
