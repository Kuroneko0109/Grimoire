#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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

void cpu_dump_info(void)
{
	thread_t * thread;
	int i;
	printf("======== Dump CPU Info ========\n");
	printf("Core Mapping : 0x%08lX\n", cpu.core_mapping);
	for(i=0;i<cpu.core_num;i++)
	{
		thread = cpu.core_thread[i];
		if(!thread)
			continue;
		thread->dump(thread);
	}
	printf("===============================\n");
}

void cpu_task_register(task_t * task, int core_idx)
{
	thread_t * thread = cpu.core_thread[core_idx];

	if(NULL != thread)
		thread->task_register(thread, task);
}

void cpu_drive(void)
{
	thread_t * thread;
	int i;

	for(i=0;i<cpu.core_num;i++)
	{
		thread = cpu.core_thread[i];
		if(thread)
			thread->run(thread);
	}

	for(i=0;1;i++)
	{
		printf("\n%d seconds...\n\n", i);
		sleep(1);
	}
}

void init_cpu(config_t * config)
{
	int i;
	char * core_bitmask;
	char * core_maxcount;
	thread_t * thread;
	core_bitmask = config->get_value(config, "core_bitmask");
	core_maxcount = config->get_value(config, "core_maxcount");

	if(NULL == core_bitmask || NULL == core_maxcount)
	{
		printf("%s(%d) : Can't get value\n", __func__, __LINE__);
		return;
	}

	cpu.core_num = atoi(core_maxcount);
	cpu.core_mapping = strtoull(core_bitmask, &core_bitmask, 16);

	cpu.core_thread = malloc(sizeof(thread_t *) * cpu.core_num);
	for(i=0;i<cpu.core_num;i++)
	{
		if(cpu.core_mapping & (1<<i))	// using core filtering
		{
			thread = create_thread(NULL);
			thread->core_bind(thread, i);
			cpu.core_thread[i] = thread;
		}
		else
			cpu.core_thread[i] = NULL;
	}

	cpu.public.drive = cpu_drive;
	cpu.public.task_register = cpu_task_register;
	cpu.public.dump_info = cpu_dump_info;
}
