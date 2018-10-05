#include <libgrimoire/security/tls_server.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <openssl/ssl.h>

typedef struct priv_tls_server priv_tls_server_t;

struct priv_tls_server {
	tls_server_t public;

	int fd;

	struct sockaddr_in addr_in;

	SSL_CTX * ctx;
};

tls_peer_t * tls_server_accept(tls_server_t * this)
{
	priv_tls_server_t * priv = (priv_tls_server_t *)this;
	SSL * ssl;
	int fd;
	socklen_t addr_len;

	fd = accept(priv->fd, (struct sockaddr_t *)&priv->addr_in, &addr_len);
	if(fd == -1)
		return NULL;

	ssl = SSL_new(priv->ctx);
	SSL_set_fd(ssl, fd);
	SSL_accept(ssl);
}

void tls_server_open(tls_server_t * this)
{
	priv_tls_server_t * priv = (priv_tls_server_t *)this;

	if(-1 != priv->fd)
		return;

	priv->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == priv->fd)
		return;

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

void tls_server_close(tls_server_t * this)
{
	priv_tls_server_t * priv = (priv_tls_server_t *)this;

	if(-1 != priv->fd)
		close(priv->fd);
}

void tls_server_set_addr(tls_server_t * this, const char * addr)
{
	priv_tls_server_t * priv = (priv_tls_server_t *)this;
	priv->addr_in.sin_family = AF_INET;
	priv->addr_in.sin_addr.s_addr = inet_addr(addr);
}

void tls_server_set_port(tls_server_t * this, uint16_t port)
{
	priv_tls_server_t * priv = (priv_tls_server_t *)this;
	priv->addr_in.sin_port = htons(port);
}

tls_server_t * create_tls_server(config_t * config)
{
	priv_tls_server_t * private;
	tls_server_t * public;

	char * value;

	private = malloc(sizeof(priv_tls_server_t));
	public = &private->public;

	private->fd = -1;
	private->ctx = SSL_CTX_new(TLSv1_2_server_method());

	value = config->get_value(config, "tls_server_cert");
	SSL_CTX_use_certificate_file(private->ctx, value, SSL_FILETYPE_PEM);
	value = config->get_value(config, "tls_server_key");
	SSL_CTX_use_PrivateKey_file(private->ctx, value, SSL_FILETYPE_PEM);

	SSL_CTX_check_private_key(private->ctx);

	public->accept = tls_server_accept;
	public->open = tls_server_open;
	public->close = tls_server_close;
	public->set_addr = tls_server_set_addr;
	public->set_port = tls_server_set_port;

	return public;
}
