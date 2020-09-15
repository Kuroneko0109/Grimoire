#include <libgrimoire/common/common.h>
#include <libgrimoire/system/memory.h>
#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/security/auth_server.h>
#include <libgrimoire/security/security_server.h>
#include <libgrimoire/security/auth.h>
#include <libgrimoire/security/sa.h>
#include <libgrimoire/security/cpkt.h>
#include <libgrimoire/config/config.h>
//#include <auth_server/auth_server.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* DH Usage
 *
 * dh_t * alice = create_dh(14);
 * alice->rand(alice);
 * alice->g_x_mod(alice);
 *
 * dh_t * bob = create_dh(14);
 * bob->rand(bob);
 * bob->g_x_mod(bob);
 *
 * alice->g_xy_mod(alice,
 * 	*bob->get_g_x_mod(bob));
 * bob->g_xy_mod(bob,
 * 	*alice->get_g_x_mod(alice));
 *
 * */

typedef struct priv_auth_server priv_auth_server_t;

struct priv_auth_server {
	auth_server_t public;

	dh_t * dh;

	config_t * accounts;

	list_t * security_server_list;
};

security_server_t * auth_server_contract_peer(auth_server_t * this, peer_t * peer)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	sa_t * sa = create_sa();
	dh_t * dh = priv->dh;

	p1_init_t p1i_msg = {0, };
	p1_resp_t p1r_msg = {0, };
	p2_init_t p2i_msg = {0, };
	uint8_t buffer[2048];
	int rc;
	cpkt_init_hdr_t init_msg = {0, };

	size_t gx_size;
	uint8_t * gx;
	uint8_t hmac_modp[64];
	uint8_t hmac_id[64];
	int i;

	size_t gxy_size;
	uint8_t * sk;
	
	cpkt_t * cpkt;

	/* receive init */
	peer->read(peer, &p1i_msg, sizeof(p1i_msg));
	p1_init_dump(&p1i_msg);

	this->set_akey_by_psk(this, sa, p1i_msg.id);
	sa->sign(sa, hmac_id, p1i_msg.id, sizeof(p1i_msg.id));
	if(0 != memcmp(hmac_id, p1i_msg.hmac_id, sizeof(p1i_msg.hmac_id)))
	{
		printf("%s(%d)\n", __func__, __LINE__);
		binary_dump("ERROR! hmac_id", hmac_id, sizeof(hmac_id));
		binary_dump("ERROR! p1i_msg.hmac_id", p1i_msg.hmac_id, sizeof(p1i_msg.hmac_id));
		sa->destroy(sa);
		peer->destroy(peer);
		return NULL;
	}

	sa->sign(sa, p1r_msg.hmac_k, p1i_msg.k, sizeof(p1i_msg.k));

	dh->rand(dh);
	dh->g_x_mod(dh);

	gx = dh->export_gx(dh, &gx_size);
	memcpy(p1r_msg.modp, gx, gx_size);
	free(gx);

	sa->set_akey(sa, p1i_msg.k, sizeof(p1i_msg.k));
	sa->sign(sa, p1r_msg.hmac_modp, p1r_msg.modp, sizeof(p1r_msg.modp));

	/* send resp */
	peer->write(peer, &p1r_msg, sizeof(p1r_msg));
	p1_resp_dump(&p1r_msg);

	/* wait */

	/* receive init */
	peer->read(peer, &p2i_msg, sizeof(p2i_msg));
	p2_init_dump(&p2i_msg);

	sa->sign(sa, hmac_modp, p2i_msg.modp, sizeof(p2i_msg.modp));
	if(0 != memcmp(hmac_modp, p2i_msg.hmac_modp, sizeof(p2i_msg.hmac_modp)))
	{
		printf("%s(%d)\n", __func__, __LINE__);
		binary_dump("ERROR! hmac_modp", hmac_modp, sizeof(hmac_modp));
		binary_dump("ERROR! p2i_msg.hmac_modp", p2i_msg.hmac_modp, sizeof(p2i_msg.hmac_modp));
		sa->destroy(sa);
		peer->destroy(peer);
		return NULL;
	}

	dh->g_xy_mod(dh, p2i_msg.modp);

	sk = dh->export_gxy(dh, &gxy_size);
	binary_dump("modp gxy", sk, gxy_size);

	sa->set_ekey(sa, sk, 32);
	free(sk);
	sa->gen_iv(sa);

	init_msg.result = 1;
	init_msg.sid = 25252;

	/* encrypt */
	rc = sa->encrypt(sa, buffer, &init_msg, sizeof(init_msg));
	cpkt = create_cpkt(CPKT_INIT, rc);
	memcpy(cpkt->payload, buffer, rc);
	binary_dump("cpkt->payload", cpkt->payload, rc);
	sa->get_iv(sa, cpkt->iv);
	binary_dump("cpkt->iv", cpkt->iv, sizeof(cpkt->iv));

	/* send resp */
	peer->write(peer, cpkt, sizeof(cpkt_t) + rc);

	return create_security_server(peer, sa, p2i_msg.type, p2i_msg.name);
}

void auth_server_set_akey_by_psk(auth_server_t * this, sa_t * sa, uint8_t * id)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	config_t * accounts = priv->accounts;

	char * psk;

	psk = accounts->get_value(accounts, id);

	sa->set_akey(sa, psk, strlen(psk));
}

auth_server_t * create_auth_server(char * account_path)
{
	priv_auth_server_t * private;
	auth_server_t * public;

	private = galloc(sizeof(priv_auth_server_t));
	public = &private->public;

	private->accounts = create_config(account_path, 8192);
	private->dh = create_dh(14);
	private->security_server_list = create_list(NULL, NULL, NULL);
	public->set_akey_by_psk = auth_server_set_akey_by_psk;
	public->contract_peer = auth_server_contract_peer;

	private->dh->rand_init(private->dh);

	return public;
}
