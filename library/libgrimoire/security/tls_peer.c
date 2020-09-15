#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <libgrimoire/system/memory.h>
#include <libgrimoire/security/tls_peer.h>

typedef struct priv_tls_peer priv_tls_peer_t;

struct priv_tls_peer {
	tls_peer_t public;

	int fd;

	struct sockaddr_in addr_in;
	SSL_CTX * ctx;
	SSL * ssl;
};

int tls_peer_read(tls_peer_t * this, void * dst, int len)
{
	priv_tls_peer_t * priv = (priv_tls_peer_t *)this;

	return SSL_read(priv->ssl, dst, len);
}

int tls_peer_write(tls_peer_t * this, const void * src, int len)
{
	priv_tls_peer_t * priv = (priv_tls_peer_t *)this;

	return SSL_write(priv->ssl, src, len);
}

void tls_peer_destroy(tls_peer_t * this)
{
	priv_tls_peer_t * priv = (priv_tls_peer_t *)this;

	if(priv->ssl)
	{
		SSL_shutdown(priv->ssl);
		SSL_free(priv->ssl);
	}

	if(-1 != priv->fd)
		close(priv->fd);

	free(this);
}

int tls_peer_connect(tls_peer_t * this)
{
	priv_tls_peer_t * priv = (priv_tls_peer_t *)this;

	priv->fd = socket(AF_INET, SOCK_STREAM, 0);

	if(-1 == connect(priv->fd, (struct sockaddr *)&priv->addr_in, sizeof(priv->addr_in)))
	{
		printf("Connect failure\n");
		return -1;
	}

	priv->ssl = SSL_new(priv->ctx);
	SSL_set_fd(priv->ssl, priv->fd);

	return priv->fd;
}

void tls_peer_set_addr(tls_peer_t * this, const char * addr_str)
{
	priv_tls_peer_t * priv = (priv_tls_peer_t *)this;

	priv->addr_in.sin_family = AF_INET;
	priv->addr_in.sin_addr.s_addr = inet_addr(addr_str);
}

void tls_peer_set_port(tls_peer_t * this, uint16_t port)
{
	priv_tls_peer_t * priv = (priv_tls_peer_t *)this;

	priv->addr_in.sin_port = htons(port);
}

tls_peer_t * create_tls_peer(SSL * ssl)
{
	priv_tls_peer_t * private;
	tls_peer_t * public;

	private = galloc(sizeof(priv_tls_peer_t));
	public = &private->public;

	public->read = tls_peer_read;
	public->write = tls_peer_write;
	public->destroy = tls_peer_destroy;
	public->set_addr = tls_peer_set_addr;
	public->set_port = tls_peer_set_port;

	if(ssl)
	{
		private->ssl = ssl;
		private->fd = SSL_get_fd(ssl);
	}
	else
	{
		private->ctx = SSL_CTX_new(TLS_client_method());
		private->fd = -1;
	}

	return public;
}
