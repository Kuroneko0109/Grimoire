#ifndef _SHARED_
#define _SHARED_

typedef struct shared shared_t;

struct shared {
	void (*declare)(shared_t * this);
	void (*get)(shared_t * this);
	void (*destroy)(shared_t * this);
};

shared_t * create_shared(void);

#endif
