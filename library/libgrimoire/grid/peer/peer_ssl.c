#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <openssl/ssl.h>
#include <arpa/inet.h>

#include <libgrimoire/grid/peer.h>

typedef struct priv_peer priv_peer_t;

struct priv_peer {
	peer_t public;

	int fd;

	struct sockaddr_in addr_in;
};

void peer_set_fd_ssl(peer_t * this, int fd)
{
	priv_peer_t * priv = (priv_peer_t *)this;

	if(-1 != priv->fd)
		this->close(this);

	priv->fd = fd;
}

void peer_set_blk_ssl(peer_t * this)
{
	priv_peer_t * priv = (priv_peer_t *)this;
	int flags;

	if(-1 != priv->fd)
	{
		flags = fcntl(priv->fd, F_GETFL, 0);
		fcntl(priv->fd, F_SETFL, flags | O_NONBLOCK);
	}
}

void peer_set_nblk_ssl(peer_t * this)
{
	priv_peer_t * priv = (priv_peer_t *)this;
	int flags;

	if(-1 != priv->fd)
	{
		flags = fcntl(priv->fd, F_GETFL, 0);
		fcntl(priv->fd, F_SETFL, flags & ~O_NONBLOCK);
	}
}

ssize_t peer_read_ssl(peer_t * this, void * dst, size_t size)
{
	priv_peer_t * priv = (priv_peer_t *)this;
	return read(priv->fd, dst, size);
}

ssize_t peer_write_ssl(peer_t * this, void * src, size_t size)
{
	priv_peer_t * priv = (priv_peer_t *)this;
	return write(priv->fd, src, size);
}

void peer_destroy_ssl(peer_t * this)
{
	priv_peer_t * priv = (priv_peer_t *)this;

	if(-1 != priv->fd)
		close(priv->fd);

	free(this);
}

int peer_open_ssl(peer_t * this)
{
	priv_peer_t * priv = (priv_peer_t *)this;
	int res;

	if(-1 != priv->fd)
		return -1;

	priv->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == priv->fd)
		return -1;

	res = connect(priv->fd, (struct sockaddr *)&priv->addr_in, sizeof(struct sockaddr_in));

	return res;
}

void peer_close_ssl(peer_t * this)
{
	priv_peer_t * priv = (priv_peer_t *)this;

	if(-1 != priv->fd)
		close(priv->fd);
	priv->fd = -1;
}

void peer_set_addr_ssl(peer_t * this, const char * addr_str)
{
	priv_peer_t * priv = (priv_peer_t *)this;

	priv->addr_in.sin_family = AF_INET;
	priv->addr_in.sin_addr.s_addr = inet_addr(addr_str);
}

void peer_set_port_ssl(peer_t * this, uint16_t port)
{
	priv_peer_t * priv = (priv_peer_t *)this;

	priv->addr_in.sin_port = htons(port);
}

void peer_dump_ssl(peer_t * this)
{
	priv_peer_t * priv = (priv_peer_t *)this;
	printf("%s(%d) %p\n", __func__, __LINE__, priv);
}

peer_t * create_peer_ssl(void)
{
	priv_peer_t * private;
	peer_t * public;

	private = malloc(sizeof(priv_peer_t));
	public = &private->public;

	private->fd = -1;
	memset(&private->addr_in, 0, sizeof(struct sockaddr_in));

	public->set_addr = peer_set_addr_ssl;
	public->set_fd = peer_set_fd_ssl;
	public->read = peer_read_ssl;
	public->write = peer_write_ssl;
	public->set_blk = peer_set_blk_ssl;
	public->set_nblk = peer_set_nblk_ssl;
	public->set_port = peer_set_port_ssl;
	public->dump = peer_dump_ssl;
	public->open = peer_open_ssl;
	public->close = peer_close_ssl;
	public->destroy = peer_destroy_ssl;

	return public;
}

void __attribute__((constructor)) init_ssl(void)
{
	printf("Load SSL Libraries\n");
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
}
