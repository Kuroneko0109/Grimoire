#include <stdlib.h>

#include <openssl/ssl.h>
#include <libgrimoire/security/tls_peer.h>

typedef struct priv_tls_peer priv_tls_peer_t;

struct priv_tls_peer {
	tls_peer_t public;

	int fd;
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
		SSL_shutdown(priv->ssl);

	if(-1 != priv->fd)
		close(priv->fd);

	free(this);
}

tls_peer_t * create_tls_peer(SSL * ssl)
{
	priv_tls_peer_t * private;
	tls_peer_t * public;

	private = malloc(sizeof(priv_tls_peer_t));
	public = &private->public;

	private->ssl = ssl;

	public->read = tls_peer_read;
	public->write = tls_peer_write;
	public->destroy = tls_peer_destroy;

	return public;
}
