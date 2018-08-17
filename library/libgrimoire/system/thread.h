#ifndef _THREAD_
#define _THREAD_

#include <libgrimoire/system/task.h>
#include <libgrimoire/datastructure/list.h>

typedef struct thread thread_t;

struct thread {
	void (*task_list_set)(thread_t * this, list_t * task_list);
	void (*core_bind)(thread_t * this, int core_num);
	void (*core_free)(thread_t * this, int core_num);
	void (*task_register)(thread_t * this, task_t * task);
	void (*execute_once)(thread_t * this);

	int (*run)(thread_t * this);
	void (*stop)(thread_t * this);

	void (*dump)(thread_t * this);
	void (*destroy)(thread_t * this);
};

thread_t * create_thread(list_t * task_list);

#endif
