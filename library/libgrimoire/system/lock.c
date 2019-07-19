#include <libgrimoire/system/lock.h>
#include <libgrimoire/system/mutex.h>
#include <libgrimoire/system/spinlock.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

lock_t * create_lock(int type)
{
	lock_t * lock;

	switch(type)
	{
		case LOCK_SPINLOCK :
			lock = create_spinlock();
			break;
		case LOCK_MUTEX :
		default :
			lock = create_mutex();
			break;
	}

	return lock;
}
