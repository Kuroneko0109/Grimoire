#include <libgrimoire/security/security_server.h>
#include <libgrimoire/security/cpkt.h>
#include <libgrimoire/security/dh.h>
#include <error.h>
#include <errno.h>

static int security_server_uid = 0;

typedef struct priv_security_server priv_security_server_t;

struct priv_security_server {
	security_server_t public;

	int uid;
	int type;
	char name[32];

	peer_t * peer;
	sa_t * sa;
};

ssize_t security_server_write(security_server_t * this, void * src, size_t size)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;
	peer_t * peer = priv->peer;
	sa_t * sa = priv->sa;
	uint8_t buffer[4096];
	cpkt_t * cpkt = buffer;
	cpkt_proxy_hdr_t * phdr = cpkt->payload;
	int rc;

	cpkt->type = CPKT_PROXY;

	sa->gen_iv(sa);
	sa->get_iv(sa, cpkt->iv);

	memcpy(phdr->payload, src, size);

	binary_dump("plain", phdr, sizeof(phdr) + size);
	rc = sa->encrypt(sa, phdr, phdr, sizeof(phdr) + size);
	cpkt->payload_len = rc;
	binary_dump("cipher", phdr, sizeof(phdr) + rc);//sizeof(phdr) + size);

//	binary_dump("pkt", buffer, sizeof(cpkt_t) + cpkt->payload_len);

	return peer->write(peer, cpkt, sizeof(cpkt_t) + cpkt->payload_len);
}

ssize_t security_server_read(security_server_t * this, void * dst, size_t size)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;
	peer_t * peer = priv->peer;
	sa_t * sa = priv->sa;
	char buffer[4096] = {0, };
	int rc;
	cpkt_t * cpkt = (cpkt_t *)buffer;

	rc = peer->read(peer, buffer, sizeof(buffer));
	if(rc < 0 || rc == 0)
	{
		if(errno != EAGAIN)
			return -1;
	}

	if(rc == sizeof(buffer))
	{
		printf("%s(%d) error\n", __func__, __LINE__);
		return -1;
	}

	rc = sizeof(cpkt_t);
	if(cpkt->type == CPKT_PROXY)
	{
		sa->set_iv(sa, cpkt->iv);
		rc += sa->decrypt(sa, cpkt->payload, cpkt->payload, cpkt->payload_len);
	}

	memcpy(dst, buffer, rc);

	return rc;
}

void security_server_rekey(security_server_t * this)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;
	dh_t * dh = create_dh(14);

	dh->destroy(dh);
}

void security_server_destroy(security_server_t * this)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;

	priv->peer->destroy(priv->peer);
	priv->sa->destroy(priv->sa);

	free(this);
}

char * security_server_get_name(security_server_t * this)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;
	return priv->name;
}

int security_server_get_type(security_server_t * this)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;
	return priv->type;
}

int security_server_get_uid(security_server_t * this)
{
	priv_security_server_t * priv = (priv_security_server_t *)this;
	return priv->uid;
}

security_server_t * create_security_server(peer_t * peer, sa_t * sa, int type, char * name)
{
	priv_security_server_t * private;
	security_server_t * public;

	private = malloc(sizeof(priv_security_server_t));
	public = &private->public;

	strcpy(private->name, name);

	private->peer = peer;
	private->sa = sa;
	private->uid = security_server_uid++;
	private->type = type;

	public->write = security_server_write;
	public->read = security_server_read;
	public->rekey = security_server_rekey;
	public->get_type = security_server_get_type;
	public->get_name = security_server_get_name;
	public->get_uid = security_server_get_uid;
	public->destroy = security_server_destroy;

	return public;
}
