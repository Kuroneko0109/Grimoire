#include <libgrimoire/system/file.h>
#include <libgrimoire/system/lock.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct priv_file priv_file_t;

struct priv_file {
	file_t public;

	int fd;

	int flags;
	int permission;

	const char * directory;

	lock_t * lock;
};

void file_creat(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	creat(priv->directory, priv->permission);
}

int file_open(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	if(-1 != priv->fd)
		return priv->fd;

	if(this->exist(this))
		this->creat(this);

	priv->fd = open(priv->directory, priv->flags);
	
	return priv->fd;
}

void file_close(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	if(-1 != priv->fd)
		this->fsync(this);

	close(priv->fd);
	priv->fd = -1;
}

int file_exist(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	return access(priv->directory, F_OK);
}

void file_fsync(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	if(-1 == priv->fd)
		return;

	fsync(priv->fd);
}

void file_destroy(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	this->close(this);
	priv->lock->destroy(priv->lock);
	free(this);
}

ssize_t file_read(file_t * this, void * dst, size_t size)
{
	priv_file_t * priv = (priv_file_t *)this;
	
	if(-1 != priv->fd)
		return read(priv->fd, dst, size);

	return -1;
}

ssize_t file_write(file_t * this, void * src, size_t size)
{
	priv_file_t * priv = (priv_file_t *)this;

	if(-1 != priv->fd)
		return write(priv->fd, src, size);

	return -1;
}

file_t * create_file(const char * filename)
{
	priv_file_t * private;
	file_t * public;

	private = malloc(sizeof(priv_file_t));
	public = &private->public;

	private->directory = filename;
	private->fd = -1;
	private->flags = O_RDWR | O_CREAT;
	private->permission = 0644;
	private->lock = create_lock(LOCK_SPINLOCK);

	public->exist = file_exist;
	public->creat = file_creat;
	public->open = file_open;
	public->close = file_close;
	public->read = file_read;
	public->write = file_write;
	public->fsync = file_fsync;
	public->destroy = file_destroy;

	return public;
}
