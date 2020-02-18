#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgrimoire/chrono/chrono.h>
#include <libgrimoire/system/task.h>

typedef struct priv_task priv_task_t;

struct priv_task {
	task_t public;

	char task_name[32];
	void * (*func)(void *);
	void (*destroyer)(void *);
	void * param;

	uint64_t execute_count;

	chrono_t * timer;
};

void * task_execute(task_t * this)
{
	priv_task_t * priv = (priv_task_t *)this;

	if(0 == this->timer_check(this))
		return NULL;

	priv->execute_count++;
	return priv->func(priv->param);
}

void task_destroy(task_t * this)
{
	priv_task_t * priv = (priv_task_t *)this;
	if(priv->destroyer)
		priv->destroyer(priv->param);
	else
		free(priv->param);
}

int task_timer_check(task_t * this)
{
	priv_task_t * priv = (priv_task_t *)this;

	return priv->timer->check_period_reset(priv->timer);
}

void * task_exec(void * param)
{
	task_t * this = (task_t *)param;
	return this->execute(this);
}

void task_timer_init(task_t * this)
{
	priv_task_t * priv = (priv_task_t *)this;

	priv->timer->start(priv->timer);
}

void task_set_period(task_t * this, uint64_t period)
{
	priv_task_t * priv = (priv_task_t *)this;

	priv->timer->set_period(priv->timer, period);
}

void task_dump(task_t * this)
{
	priv_task_t * priv = (priv_task_t *)this;

	printf("Task(%s) %lu'th execute\n", priv->task_name, priv->execute_count);
}

task_t * create_task(char * task_name, void * (*func)(void *), void * param, void (*destroyer)(void *))
{
	priv_task_t * private;
	task_t * public;

	private = malloc(sizeof(priv_task_t));
	public = &private->public;

	strcpy(private->task_name, task_name);
	private->func = func;
	private->param = param;
	private->destroyer = destroyer;
	private->execute_count = 0;

	public->set_period = task_set_period;
	public->timer_init = task_timer_init;
	public->execute = task_execute;
	public->timer_check = task_timer_check;
	public->dump = task_dump;
	public->destroy = task_destroy;

	private->timer = create_chrono();

	return public;
}
