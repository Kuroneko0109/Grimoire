#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <libgrimoire/grid/acceptor.h>
#include <libgrimoire/grid/peer.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_acceptor priv_acceptor_t;

struct priv_acceptor {
	acceptor_t public;

	int fd;

	struct sockaddr_un addr_un;
};

void acceptor_set_addr_uds(acceptor_t * this, const char * addr)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;

	priv->addr_un.sun_family = AF_UNIX;
	strcpy(priv->addr_un.sun_path, addr);
}

void acceptor_set_port_uds(acceptor_t * this, uint16_t port)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;
	printf("%s(%d) %p %d\n", __func__, __LINE__, priv, port);
}

int acceptor_open_uds(acceptor_t * this)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;
	char * path = priv->addr_un.sun_path;

	priv->fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(priv->fd < 0)
		return priv->fd;

	if((access(path, F_OK) == 0))
		unlink(path);

bind_retry :
	if(0 > bind(priv->fd, (struct sockaddr *)&priv->addr_un, sizeof(priv->addr_un)))
	{
		sleep(1);
		goto bind_retry;
	}

	if(0 > listen(priv->fd, 0))
		this->close(this);

	return priv->fd;
}

void acceptor_close_uds(acceptor_t * this)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;
	char * path = priv->addr_un.sun_path;

	if(priv->fd != -1)
		close(priv->fd);
	priv->fd = -1;

	if((access(path, F_OK) == 0))
		unlink(path);
}

void acceptor_destroy_uds(acceptor_t * this)
{
	this->close(this);
	free(this);
}

int acceptor_accept_uds(acceptor_t * this)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;

	struct sockaddr_in addr_in;
	socklen_t addr_len;

	return accept(priv->fd, (struct sockaddr *)&addr_in, &addr_len);
}

peer_t * acceptor_accept_peer_uds(acceptor_t * this)
{
	int fd;
	peer_t * peer = NULL;

	fd = this->accept(this);
	if(-1 == fd)
		return peer;

	peer = create_peer_uds();
	peer->set_fd(peer, fd);

	return peer;
}

acceptor_t * create_acceptor_uds(void)
{
	priv_acceptor_t * private;
	acceptor_t * public;

	private = malloc(sizeof(priv_acceptor_t));
	public = &private->public;

	public->set_addr = acceptor_set_addr_uds;
	public->set_port = acceptor_set_port_uds;
	public->open = acceptor_open_uds;
	public->close = acceptor_close_uds;
	public->accept = acceptor_accept_uds;
	public->accept_peer = acceptor_accept_peer_uds;
	public->destroy = acceptor_destroy_uds;

	return public;
}
