#ifndef _LOCK_
#define _LOCK_

typedef struct lock lock_t;

struct lock {
	void (*lock)(lock_t * this);
	void (*unlock)(lock_t * this);
	void (*destroy)(lock_t * this);
};

lock_t * create_lock(void);

#endif
