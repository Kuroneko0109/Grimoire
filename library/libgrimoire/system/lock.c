#include <libgrimoire/system/lock.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct priv_lock priv_lock_t;

struct priv_lock {
	lock_t public;

	pthread_mutex_t mutex;
};

void lock_lock(lock_t * this)
{
	priv_lock_t * priv = (priv_lock_t *)this;

	pthread_mutex_lock(&priv->mutex);
}

void lock_unlock(lock_t * this)
{
	priv_lock_t * priv = (priv_lock_t *)this;

	pthread_mutex_unlock(&priv->mutex);
}

void lock_destroy(lock_t * this)
{
	priv_lock_t * priv = (priv_lock_t *)this;

	pthread_mutex_destroy(&priv->mutex);
}

lock_t * create_lock(void)
{
	priv_lock_t * private;
	lock_t * public;

	private = malloc(sizeof(priv_lock_t));
	public = &private->public;

	pthread_mutex_init(&private->mutex, NULL);

	public->lock = lock_lock;
	public->unlock = lock_unlock;
	public->destroy = lock_destroy;

	public->unlock(public);

	return public;
}
