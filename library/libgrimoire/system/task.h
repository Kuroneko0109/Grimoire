#ifndef _TASK_
#define _TASK_

#include <stdint.h>

typedef struct task task_t;

struct task {
	void (*timer_init)(task_t * this);
	void (*set_period)(task_t * this, uint64_t interval);
	void * (*execute)(task_t * this);
	int (*timer_check)(task_t * this);
	void (*dump)(task_t * this);
	void (*destroy)(task_t * this);
};

task_t * create_task(char * task_name, void * (*func)(void *), void * param, void (*destroyer)(void *));
void * task_exec(void * this);

#endif
