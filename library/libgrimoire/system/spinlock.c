#include <libgrimoire/system/spinlock.h>
#include <libgrimoire/system/memory.h>

#include <stdlib.h>
#include <pthread.h>

typedef struct priv_spinlock priv_spinlock_t;
struct priv_spinlock {
	lock_t public;

	pthread_spinlock_t lock;
};

void spinlock_lock(lock_t * this)
{
	priv_spinlock_t * priv = (priv_spinlock_t *)this;

	pthread_spin_lock(&priv->lock);
}

void spinlock_unlock(lock_t * this)
{
	priv_spinlock_t * priv = (priv_spinlock_t *)this;

	pthread_spin_unlock(&priv->lock);
}

void spinlock_destroy(lock_t * this)
{
	free(this);
}

lock_t * create_spinlock(void)
{
	priv_spinlock_t * private;
	lock_t * public;

	private = galloc(sizeof(priv_spinlock_t));
	public = &private->public;

	pthread_spin_init(&private->lock, 0);

	public->lock = spinlock_lock;
	public->unlock = spinlock_unlock;
	public->destroy = spinlock_destroy;

	return public;
}
