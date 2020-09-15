#include <libgrimoire/system/file.h>
#include <libgrimoire/system/memory.h>
#include <libgrimoire/system/lock.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct priv_file priv_file_t;

struct priv_file {
	file_t public;

	int fd;

	int flags;
	int permission;

	char path[512];

	lock_t * lock;
};

void file_creat(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	creat(priv->path, priv->permission);
}

int file_open(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;

	if(-1 != priv->fd)
		return priv->fd;

	if(this->exist(this))
		this->creat(this);

	priv->fd = open(priv->path, priv->flags);
	
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

	return access(priv->path, F_OK);
}

void file_set_path(file_t * this, const char * path)
{
	priv_file_t * priv = (priv_file_t *)this;

	if(path)
		strcpy(priv->path, path);
	else
		printf("%s(%d) ERROR : path is NULL\n", __func__, __LINE__);
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
	ssize_t ret = -1;
	
	if(-1 != priv->fd)
	{
		priv->lock->lock(priv->lock);
		ret = read(priv->fd, dst, size);
		priv->lock->unlock(priv->lock);
	}

	return ret;
}

ssize_t file_write(file_t * this, void * src, size_t size)
{
	priv_file_t * priv = (priv_file_t *)this;
	ssize_t ret = -1;

	if(-1 != priv->fd)
	{
		priv->lock->lock(priv->lock);
		ret = write(priv->fd, src, size);
		priv->lock->unlock(priv->lock);
	}

	return ret;
}

ssize_t file_size(file_t * this)
{
	priv_file_t * priv = (priv_file_t *)this;
	struct stat fstat;

	if(0 > stat(priv->path, &fstat))
		return -1;

	return fstat.st_size;
}

int file_seek(file_t * this, off_t off, int whence)
{
	priv_file_t * priv = (priv_file_t *)this;

	return lseek(priv->fd, off, whence);
}

file_t * create_file(const char * path)
{
	priv_file_t * private;
	file_t * public;

	private = galloc(sizeof(priv_file_t));
	public = &private->public;
	memset(private, 0, sizeof(priv_file_t));

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
	public->seek = file_seek;
	public->set_path = file_set_path;
	public->fsync = file_fsync;
	public->size = file_size;
	public->destroy = file_destroy;

	public->set_path(public, path);

	return public;
}
