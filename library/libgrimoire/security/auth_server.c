#include <libgrimoire/common/common.h>
#include <libgrimoire/security/auth_server.h>
#include <libgrimoire/security/auth.h>
#include <libgrimoire/security/sa.h>
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
	sa_t * sa;

	config_t * accounts;
};

int auth_server_contract_peer(auth_server_t * this, peer_t * peer)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	sa_t * sa = priv->sa;
	dh_t * dh = priv->dh;

	p1_init_t p1i_msg = {0, };
	p1_resp_t p1r_msg = {0, };
	p2_init_t p2i_msg = {0, };
	p2_resp_t p2r_msg = {0, };

	size_t gx_size;
	uint8_t * gx;
	uint8_t hmac_modp[64];
	uint8_t hmac_id[64];
	int i;

	/* receive init */
	peer->read(peer, &p1i_msg, sizeof(p1i_msg));
	p1_init_dump(&p1i_msg);

	this->set_akey_by_psk(this, p1i_msg.id);
	sa->sign(sa, hmac_id, p1i_msg.id, sizeof(p1i_msg.id));
	if(0 != memcmp(hmac_id, p1i_msg.hmac_id, sizeof(p1i_msg.hmac_id)))
		return -1;

	sa->sign(sa, p1r_msg.hmac_k, p1i_msg.k, sizeof(p1i_msg.k));

	dh->rand(dh);
	dh->g_x_mod(dh);

	gx = dh->export(dh, &gx_size);
	memcpy(p1r_msg.modp, gx, gx_size);
	free(gx);

	this->set_akey(this, p1i_msg.k, sizeof(p1i_msg.k));
	sa->sign(sa, p1r_msg.hmac_modp, p1r_msg.modp, sizeof(p1r_msg.modp));

	/* send resp */
	peer->write(peer, &p1r_msg, sizeof(p1r_msg));
	p1_resp_dump(&p1r_msg);

	/* wait */

	/* receive init */
	peer->read(peer, &p2i_msg, sizeof(p2i_msg));
	p2_init_dump(&p2i_msg);

	/* send resp */
	peer->write(peer, &p1r_msg, sizeof(p1r_msg));

	return 0;
}

void auth_server_set_ekey(auth_server_t * this, uint8_t * key, int klen)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	sa_t * sa = priv->sa;

	sa->set_ekey(sa, key, klen);
}

void auth_server_set_akey(auth_server_t * this, uint8_t * key, int klen)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	sa_t * sa = priv->sa;

	sa->set_akey(sa, key, klen);
}

void auth_server_set_akey_by_psk(auth_server_t * this, uint8_t * id)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	sa_t * sa = priv->sa;
	config_t * accounts = priv->accounts;

	char * psk;

	psk = accounts->get_value(accounts, id);

	sa->set_akey(sa, psk, strlen(psk));
}

auth_server_t * create_auth_server(char * account_path)
{
	priv_auth_server_t * private;
	auth_server_t * public;

	private = malloc(sizeof(priv_auth_server_t));
	public = &private->public;

	private->accounts = create_config(account_path, 8192);
	private->dh = create_dh(14);
	private->sa = create_sa();
	public->set_akey_by_psk = auth_server_set_akey_by_psk;
	public->set_ekey = auth_server_set_ekey;
	public->set_akey = auth_server_set_akey;
	public->contract_peer = auth_server_contract_peer;

	private->dh->rand_init(private->dh);

	return public;
}
