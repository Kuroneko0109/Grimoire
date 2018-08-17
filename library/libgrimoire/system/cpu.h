#ifndef _CPU_
#define _CPU_

#include <libgrimoire/system/task.h>
#include <libgrimoire/datastructure/list.h>

#define MAX_CPU_CORES	(64)

typedef struct cpu cpu_t;

struct cpu {
	void (*dump_info)(cpu_t * this);
	void (*task_register)(cpu_t * this, task_t * task, int core_idx);
	void (*drive)(cpu_t * this);
};

void init_cpu(void);
cpu_t * get_global_cpu(void);

#endif
