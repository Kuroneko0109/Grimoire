#ifndef _LOCK_
#define _LOCK_

typedef struct lock lock_t;

struct lock {
#define LOCK_MUTEX	(0)
#define LOCK_SPINLOCK	(1)
	void (*lock)(lock_t * this);
	void (*unlock)(lock_t * this);
	void (*destroy)(lock_t * this);
};

lock_t * create_lock(int type);

#endif
