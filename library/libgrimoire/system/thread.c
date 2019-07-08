#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>

#include <libgrimoire/system/task.h>
#include <libgrimoire/system/thread.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_thread priv_thread_t;

struct priv_thread {
	thread_t public;

	list_t * task_list;

	cpu_set_t cpuset;

	int state;
	int pthread_id;
	pthread_t pthread;
	void * (*thread_driver)(void *);
};

void thread_task_list_set(thread_t * this, list_t * task_list)
{
	priv_thread_t * priv = (priv_thread_t *)this;
	list_t * old_task_list = priv->task_list;

	if(old_task_list)
		old_task_list->destroy(old_task_list);

	priv->task_list = task_list;
}

void thread_task_register(thread_t * this, task_t * task)
{
	priv_thread_t * priv = (priv_thread_t *)this;
	list_t * task_list = priv->task_list;
	
	if(priv->task_list)
	{
		task_list->lock(task_list);
		task_list->enqueue_data(task_list, task);
		task_list->unlock(task_list);
	}
}

void * thread_invoke(void * param)
{
	thread_t * this = (thread_t *)param;
	priv_thread_t * priv = (priv_thread_t *)param;

	while(priv->state)
		this->execute_once(this);

	return param;
}

void thread_destroy(thread_t * this)
{
	priv_thread_t * priv = (priv_thread_t *)this;
	list_t * task_list = priv->task_list;

	this->stop(this);

	task_list->destroy(task_list);
}

int thread_run(thread_t * this)
{
	priv_thread_t * priv = (priv_thread_t *)this;
	list_t * task_list = priv->task_list;
	task_t * task;
	iterator_t * iterator;
	int count;
	int i;

	task_list->lock(task_list);
	iterator = task_list->get_iterator(task_list);
	count = iterator->get_count(iterator);
	for(i=0;i<count;i++)
	{
		task = iterator->get_data(iterator, i);
		task->timer_init(task);
	}
	iterator->destroy(iterator);
	task_list->unlock(task_list);

	priv->state = 1;
	priv->pthread_id = pthread_create(
			&priv->pthread, NULL, priv->thread_driver, priv);

	pthread_setaffinity_np(priv->pthread,
			sizeof(cpu_set_t),
			&priv->cpuset);

	return priv->pthread_id;
}

void thread_stop(thread_t * this)
{
	priv_thread_t * priv = (priv_thread_t *)this;

	priv->state = 0;
	pthread_detach(priv->pthread);
}

void thread_dump(thread_t * this)
{
	priv_thread_t * priv = (priv_thread_t *)this;
	list_t * task_list = priv->task_list;
	iterator_t * iterator;
	task_t * task;

#if 0
	printf("Thread Info -> cpu : %x\n",
			pthread_getaffinity_np(
				priv->pthread,
				sizeof(cpu_set_t),
				&priv->cpuset));
#endif
	task_list->lock(task_list);
	iterator = task_list->get_iterator(task_list);

	while((task = iterator->next(iterator)))
		task->dump(task);

	iterator->destroy(iterator);
	task_list->unlock(task_list);
}

void thread_execute_once(thread_t * this)
{
	priv_thread_t * priv = (priv_thread_t *)this;
	list_t * task_list = priv->task_list;

	task_list->lock(task_list);
	task_list->foreach(task_list, task_exec);
	task_list->unlock(task_list);
}

void thread_core_bind(thread_t * this, int core_mask)
{
	priv_thread_t * priv = (priv_thread_t *)this;

	CPU_ZERO(&priv->cpuset);
	CPU_SET(core_mask, &priv->cpuset);
}

thread_t * create_thread(list_t * task_list)
{
	priv_thread_t * private;
	thread_t * public;

	private = malloc(sizeof(priv_thread_t));
	public = &private->public;

	public->task_list_set = thread_task_list_set;
	public->task_register = thread_task_register;
	public->execute_once = thread_execute_once;
	public->stop = thread_stop;
	public->dump = thread_dump;
	public->destroy = thread_destroy;
	public->run = thread_run;
	public->core_bind = thread_core_bind;

	if(NULL == task_list)
		private->task_list = create_list(NULL, NULL, NULL);
	else
		private->task_list = task_list;

	private->thread_driver = thread_invoke;

	public->core_bind(public, 0);

	public->dump(public);

	return public;
}
