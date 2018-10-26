#include <libgrimoire/security/security_client.h>
#include <libgrimoire/security/cpkt.h>
#include <libgrimoire/security/dh.h>
#include <libgrimoire/datastructure/list.h>

#include <jansson.h>
#include <error.h>
#include <errno.h>

typedef struct service_info service_info_t;

struct service_info {
	char name[32];
	int uid;
};

typedef struct priv_security_client priv_security_client_t;

struct priv_security_client {
	security_client_t public;

	int mode;

	list_t * services;
	peer_t * peer;
	sa_t * sa;
};

ssize_t security_client_write(security_client_t * this, int id, void * src, size_t size)
{
	priv_security_client_t * priv = (priv_security_client_t *)this;
	peer_t * peer = priv->peer;
	sa_t * sa = priv->sa;

	uint8_t buffer[4096] = {0, };
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
	if(rc < 0 || rc == 0)
	{
		if(errno != EAGAIN)
			return -1;
	}

	binary_dump("pkt", buffer, sizeof(cpkt_t) + cpkt->payload_len);

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

void security_client_request_service(security_client_t * this)
{
	priv_security_client_t * priv = (priv_security_client_t *)this;
	peer_t * peer = priv->peer;
	uint8_t buffer[4096];
	cpkt_t * cpkt = (cpkt_t *)buffer;
	cpkt_proxy_hdr_t * phdr;
	phdr = cpkt->payload;
	int rc;
	size_t index;
	json_error_t err;
	json_t * json;
	json_t * service_array;
	json_t * svc;

	service_info_t * info;
	list_t * services = priv->services;

	cpkt->type = CPKT_GET_SERVICES;

	peer->write(peer, cpkt, sizeof(cpkt_t));

	rc = this->read(this, buffer, sizeof(buffer));

	json = json_loads(phdr->payload, 0, &err);
	service_array = json_object_get(json, "SERVICE_LIST");

	services->flush(services);

	json_array_foreach(service_array, index, svc)
	{
		char * name;
		int uid;
		info = malloc(sizeof(service_info_t));
		json_unpack(svc, "{s:s, s:i}", "name", &name, "uid", &uid);
		strcpy(info->name, name);
		info->uid = uid;
		printf("svc %d : %s\n", index, json_dumps(svc, JSON_ENCODE_ANY));
		printf("info : %s, %d\n", info->name, info->uid);
		services->enqueue_data(services, info);
	}
}

int security_client_compare_service_info(void * _s, void * _d)
{
	service_info_t * s;
	service_info_t * d;

	return strcmp(s->name, d->name);
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
	private->services = create_list(
			NULL,
			security_client_compare_service_info,
			NULL);

	public->write = security_client_write;
	public->read = security_client_read;
	public->request_service = security_client_request_service;
	public->rekey = security_client_rekey;
	public->destroy = security_client_destroy;

	return public;
}
