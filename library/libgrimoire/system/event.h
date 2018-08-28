#ifndef _EVENT_
#define _EVENT_

typedef struct event_ctrl event_ctrl_t;

struct event_ctrl {
	void (*register)(event_ctrl_t * this, char * id, int fd, void * (*func)(void *));
	void event_ctrl_poll(void * param);
};

event_ctrl_t * get_event_ctrl_global(void);

#endif
