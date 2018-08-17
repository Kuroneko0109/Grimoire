#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <libgrimoire/config/config.h>
#include <libgrimoire/system/cpu.h>
#include <libgrimoire/system/task.h>
#include <libgrimoire/system/thread.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_cpu priv_cpu_t;

struct priv_cpu {
	cpu_t public;

	int core_num;
	thread_t ** core_thread;
	uint64_t core_mapping;
};

priv_cpu_t cpu;

cpu_t * get_global_cpu(void)
{
	return &cpu.public;
}

void cpu_dump_info(cpu_t * this)
{
}

void cpu_task_register(cpu_t * this, task_t * task, int core_idx)
{
	priv_cpu_t * priv = (priv_cpu_t *)this;
	thread_t * thread = priv->core_thread[core_idx];

	if(thread)
		thread->task_register(thread, task);
	else
		;
}

void cpu_drive(cpu_t * this)
{
	priv_cpu_t * priv = (priv_cpu_t *)this;
	thread_t * thread;
	int i;

	for(i=0;i<cpu.core_num;i++)
	{
		thread = cpu.core_thread[i];
		if(thread)
			thread->run(thread);
	}
}

#if 0
void init_cpu(void)
{
	config_t * config = get_global_config();
	char * p;
	int i;

	cpu.core_num = atoi(config->core_num);
	cpu.core_mapping = strtoull(config->core_mapping, &p, 16);

	for(i=0;i<cpu.core_num;i++)
	{
		if(cpu.core_mapping & (1<<i))	// using core filtering
			cpu.core_thread[i] = create_thread(create_list(NULL, NULL, NULL));
		else
			cpu.core_thread[i] = NULL;
	}

	cpu.public.drive = cpu_drive;
	cpu.public.task_register = cpu_task_register;
	cpu.public.dump_info = cpu_dump_info;
}
#endif
