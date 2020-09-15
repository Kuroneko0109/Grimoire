#include <libgrimoire/system/mutex.h>
#include <libgrimoire/system/memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct priv_lock priv_mutex_t;

struct priv_lock {
	lock_t public;

	pthread_mutex_t mutex;
};

void mutex_lock(lock_t * this)
{
	priv_mutex_t * priv = (priv_mutex_t *)this;

	pthread_mutex_lock(&priv->mutex);
}

void mutex_unlock(lock_t * this)
{
	priv_mutex_t * priv = (priv_mutex_t *)this;

	pthread_mutex_unlock(&priv->mutex);
}

void mutex_destroy(lock_t * this)
{
	priv_mutex_t * priv = (priv_mutex_t *)this;

	pthread_mutex_destroy(&priv->mutex);

	free(this);
}

lock_t * create_mutex(void)
{
	priv_mutex_t * private;
	lock_t * public;

	private = galloc(sizeof(priv_mutex_t));
	public = &private->public;

	pthread_mutex_init(&private->mutex, NULL);

	public->lock = mutex_lock;
	public->unlock = mutex_unlock;
	public->destroy = mutex_destroy;

	public->unlock(public);

	return public;
}
