#include <libgrimoire/common/common.h>
#include <libgrimoire/security/auth_client.h>
#include <libgrimoire/security/auth.h>
#include <libgrimoire/security/sa.h>
//#include <auth_client/auth_client.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
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

typedef struct priv_auth_client priv_auth_client_t;

struct priv_auth_client {
	auth_client_t public;

	dh_t * dh;
	sa_t * sa;

	char * id;
};

int auth_client_contract_peer(auth_client_t * this, peer_t * peer)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	sa_t * sa = priv->sa;
	dh_t * dh = priv->dh;

	p1_init_t p1i_msg = {0, };
	p1_resp_t p1r_msg = {0, };
	p2_init_t p2i_msg = {0, };
	p2_resp_t p2r_msg = {0, };

	int gx_size;
	uint8_t * gx;
	uint8_t hmac_modp[64];
	uint8_t hmac_id[64];
	int i;

	/* Random Key */
	srand(time(NULL));
	for(i=0;i<sizeof(p1i_msg.k);i++)
		p1i_msg.k[i] = (uint8_t)rand();

	strcpy(p1i_msg.id, priv->id);
	p1i_msg.group = 0;

	sa->sign(sa, p1i_msg.hmac_id, p1i_msg.id, sizeof(p1i_msg.id));
	/* send init */
	peer->write(peer, &p1i_msg, sizeof(p1i_msg));
	p1_init_dump(&p1i_msg);

	/* wait */

	/* receive resp */
	peer->read(peer, &p1r_msg, sizeof(p1r_msg));
	p1_resp_dump(&p1r_msg);

	this->set_akey(this, p1i_msg.k, sizeof(p1i_msg.k));
	sa->sign(sa, hmac_modp, p1r_msg.modp, sizeof(p1r_msg.modp));
	if(0 != memcmp(hmac_modp, p1r_msg.hmac_modp, sizeof(p1r_msg.hmac_modp)))
	{
		binary_dump("hmac_modp", hmac_modp, sizeof(hmac_modp));
		binary_dump("p1r_msg.hmac_modp", p1r_msg.hmac_modp, sizeof(p1r_msg.hmac_modp));
		return -1;
	}

	dh->rand(dh);
	dh->g_x_mod(dh);
	gx = dh->export(dh, &gx_size);
	memcpy(p2i_msg.modp, gx, gx_size);
	free(gx);

	dh->g_xy_mod(dh, p1r_msg.modp);

	sa->sign(sa, p2i_msg.hmac_modp, p2i_msg.modp, sizeof(p2i_msg.modp));

	/* send init */
	peer->write(peer, &p2i_msg, sizeof(p2i_msg));
	p2_init_dump(&p2i_msg);

	/* receive resp */
	peer->read(peer, &p2r_msg, sizeof(p2r_msg));

	return 0;
}

void auth_client_set_psk(auth_client_t * this, char * id, uint8_t * psk)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	sa_t * sa = priv->sa;

	priv->id = id;
	sa->set_akey(sa, psk, strlen(psk));
}

void auth_client_set_ekey(auth_client_t * this, uint8_t * key, int klen)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	sa_t * sa = priv->sa;

	sa->set_ekey(sa, key, klen);
}

void auth_client_set_akey(auth_client_t * this, uint8_t * key, int klen)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	sa_t * sa = priv->sa;

	sa->set_akey(sa, key, klen);
}

auth_client_t * create_auth_client(void)
{
	priv_auth_client_t * private;
	auth_client_t * public;

	private = malloc(sizeof(priv_auth_client_t));
	public = &private->public;

	private->dh = create_dh(14);
	private->sa = create_sa();
	public->set_psk = auth_client_set_psk;
	public->set_ekey = auth_client_set_ekey;
	public->set_akey = auth_client_set_akey;
	public->contract_peer = auth_client_contract_peer;

	private->dh->rand_init(private->dh);

	return public;
}
