#ifndef _CPU_
#define _CPU_

#include <libgrimoire/config/config.h>
#include <libgrimoire/system/task.h>
#include <libgrimoire/datastructure/list.h>

typedef struct cpu cpu_t;

struct cpu {
	void (*dump_info)(void);
	void (*task_register)(task_t * task, int core_idx);
	void (*drive)(void);
};

void init_cpu(config_t *);

cpu_t * get_global_cpu(void);

#endif
