#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/system/event.h>
#include <stdlib.h>
#include <string.h>

typedef struct event event_t;

struct event {
	int fd;
	void (*callback)(int);
	char * id;
};

int event_id_compare(void * _s, void * _d)
{
	event_t * s = (event_t *)_s;
	event_t * d = (event_t *)_d;

	return strcmp(s->id, d->id);
}

event_t * create_event(char * id, int fd, void * (*callback)(void *))
{
	event_t * event = malloc(sizeof(event_t));
	event->id = id;
	event->fd = fd;
	event->callback = callback;
	return event;
}

typedef struct priv_event_ctrl priv_event_ctrl_t;

struct priv_event_ctrl {
	event_ctrl_t public;

	list_t * event_list;
};

void * event_ctrl_watch(void * param)
{
	event_t * event = param;
	event->callback(event->fd);
}

void event_ctrl_poll(event_ctrl_t * this)
{
	priv_event_ctrl_t * priv = (priv_event_ctrl_t *)this;
	list_t * list = priv->event_list;

	list->foreach(list, event_ctrl_watch);
}

void event_ctrl_register_event(event_ctrl_t * this, char * id, int fd, void (*func)(int *))
{
	priv_event_ctrl_t * priv = (priv_event_ctrl_t *)this;
	list_t * list = priv->event_list;
	event_t * event;

	event = create_event(id, fd, func);

	list->lock(list);
	list->enqueue_data(list, event);
	list->unlock(list);
}

priv_event_ctrl_t * event_ctrl_global;

void __attribute__((constructor)) init_event_ctrl_global(void)
{
	event_ctrl_global = malloc(sizeof(priv_event_ctrl_t));
	priv_event_ctrl_t * private;
	event_ctrl_t * public;

	private = event_ctrl_global;
	public = &event_ctrl_global->public;

	private->event_list = create_list(NULL, NULL, NULL);

	public->register_event = event_ctrl_register_event;

	return public;
}

event_ctrl_t * get_event_ctrl_global(void)
{
	return &event_ctrl_global->public;
}
