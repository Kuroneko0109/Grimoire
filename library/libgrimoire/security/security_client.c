#include <libgrimoire/security/security_client.h>
#include <libgrimoire/security/cpkt.h>
#include <libgrimoire/security/dh.h>

typedef struct priv_security_client priv_security_client_t;

struct priv_security_client {
	security_client_t public;

	int mode;
	peer_t * peer;
	sa_t * sa;
};

ssize_t security_client_write(security_client_t * this, int id, void * src, size_t size)
{
	priv_security_client_t * priv = (priv_security_client_t *)this;
	peer_t * peer = priv->peer;
	sa_t * sa = priv->sa;
	uint8_t buffer[4096];
	cpkt_t * cpkt = buffer;
	cpkt_proxy_hdr_t * phdr = cpkt->payload;
	int rc;

	cpkt->type = CPKT_PROXY;
	phdr->service_id = id;

	sa->gen_iv(sa);
	sa->get_iv(sa, cpkt->iv);

	memcpy(phdr->payload, src, size);
	rc = sa->encrypt(sa, phdr, phdr, sizeof(phdr) + size);
	cpkt->payload_len = rc;

	return peer->write(peer, cpkt, sizeof(cpkt_t) + cpkt->payload_len);
}

ssize_t security_client_read(security_client_t * this, void * dst, size_t size)
{
	priv_security_client_t * priv = (priv_security_client_t *)this;
	peer_t * peer = priv->peer;
	sa_t * sa = priv->sa;
	char buffer[4096];
	int rc;
	cpkt_t * cpkt = (cpkt_t *)buffer;
	cpkt_proxy_hdr_t * phdr;

	rc = peer->read(peer, buffer, sizeof(buffer));

	if(rc == sizeof(buffer))
	{
		printf("%s(%d) error\n", __func__, __LINE__);
		return -1;
	}

	sa->set_iv(sa, cpkt->iv);
	rc = sa->decrypt(sa, cpkt->payload, cpkt->payload, cpkt->payload_len);
	phdr = (cpkt_proxy_hdr_t *)(cpkt->payload);
	memcpy(dst, phdr->payload, rc - sizeof(cpkt_proxy_hdr_t));

	return rc;
}

void security_client_rekey(security_client_t * this)
{
	priv_security_client_t * priv = (priv_security_client_t *)this;
	dh_t * dh = create_dh(14);

	dh->destroy(dh);
}

void security_client_destroy(security_client_t * this)
{
	priv_security_client_t * priv = (priv_security_client_t *)this;

	priv->peer->destroy(priv->peer);
	priv->sa->destroy(priv->sa);

	free(this);
}

security_client_t * create_security_client(int mode, peer_t * peer, sa_t * sa)
{
	priv_security_client_t * private;
	security_client_t * public;

	private = malloc(sizeof(priv_security_client_t));
	public = &private->public;

	private->mode = mode;
	private->peer = peer;
	private->sa = sa;

	public->write = security_client_write;
	public->read = security_client_read;
	public->rekey = security_client_rekey;
	public->destroy = security_client_destroy;

	return public;
}
