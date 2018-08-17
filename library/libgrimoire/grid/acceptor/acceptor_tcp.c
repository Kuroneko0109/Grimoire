#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <libgrimoire/grid/acceptor.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_acceptor priv_acceptor_t;

struct priv_acceptor {
	acceptor_t public;

	int fd;

	struct sockaddr_in addr_in;
};

void acceptor_set_addr_tcp(acceptor_t * this, const char * addr)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;
	priv->addr_in.sin_addr.s_addr = inet_addr(addr);
}

void acceptor_set_port_tcp(acceptor_t * this, uint16_t port)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;
	priv->addr_in.sin_port = htons(port);
}

int acceptor_open_tcp(acceptor_t * this)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;

	priv->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == priv->fd)
		return priv->fd;

	priv->addr_in.sin_family = AF_INET;

bind_retry :
	if(0 > bind(priv->fd, (struct sockaddr *)&priv->addr_in, sizeof(priv->addr_in)))
	{
		sleep(1);
		goto bind_retry;
	}

	if(0 > listen(priv->fd, 0))
		this->close(this);

	return priv->fd;
}

void acceptor_close_tcp(acceptor_t * this)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;

	if(priv->fd != -1)
		close(priv->fd);
	priv->fd = -1;
}

void acceptor_destroy_tcp(acceptor_t * this)
{
	this->close(this);
	free(this);
}

int acceptor_accept_tcp(acceptor_t * this)
{
	priv_acceptor_t * priv = (priv_acceptor_t *)this;

	struct sockaddr_in addr_in;
	socklen_t addr_len;

	return accept(priv->fd, (struct sockaddr *)&addr_in, &addr_len);
}

peer_t * acceptor_accept_peer_tcp(acceptor_t * this)
{
	int fd;
	peer_t * peer = NULL;

	fd = this->accept(this);
	if(-1 == fd)
		return peer;

	peer = create_peer_tcp();
	peer->set_fd(peer, fd);

	return peer;
}

acceptor_t * create_acceptor_tcp(void)
{
	priv_acceptor_t * private;
	acceptor_t * public;

	private = malloc(sizeof(priv_acceptor_t));
	public = &private->public;

	public->set_addr = acceptor_set_addr_tcp;
	public->set_port = acceptor_set_port_tcp;
	public->open = acceptor_open_tcp;
	public->close = acceptor_close_tcp;
	public->accept = acceptor_accept_tcp;
	public->accept_peer = acceptor_accept_peer_tcp;
	public->destroy = acceptor_destroy_tcp;

	return public;
}
