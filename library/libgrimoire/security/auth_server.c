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
	p1_init_t init_msg = {0, };
	p1_resp_t resp_msg = {0, };

	uint8_t k[64];
	uint8_t hmac_id[64];
	int i;

	peer->read(peer, &init_msg, sizeof(init_msg));

	this->set_psk(this, init_msg.id);
	sa->sign(sa, hmac_id, init_msg.id, sizeof(init_msg.id));

	printf("hmac_id : ");
	for(i=0;i<sizeof(hmac_id);i++)
		printf("%02x", hmac_id[i]);
	printf("\n");
	printf("init_msg.hmac_id : ");
	for(i=0;i<sizeof(init_msg.hmac_id);i++)
		printf("%02x", init_msg.hmac_id[i]);
	printf("\n");

	peer->write(peer, &resp_msg, sizeof(resp_msg));

	return this->verify_peer(this, peer);
}

int auth_server_verify_peer(auth_server_t * this, peer_t * peer)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	dh_t * dh = priv->dh;

	uint8_t buffer[1024];

	size_t gx_size;
	uint8_t * gx_modp = NULL;

	peer->read(peer, buffer, sizeof(buffer));

	dh->rand(dh);

	dh->g_x_mod(dh);
	dh->g_xy_mod(dh, buffer);

	gx_modp = (uint8_t *)dh->export(dh, &gx_size);

	peer->write(peer, gx_modp, gx_size);

	dh->dump(dh);

	if(gx_modp)
		free(gx_modp);

	return 0;
}

void auth_server_set_psk(auth_server_t * this, uint8_t * id)
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
	public->set_psk = auth_server_set_psk;
	public->verify_peer = auth_server_verify_peer;
	public->contract_peer = auth_server_contract_peer;

	private->dh->rand_init(private->dh);

	return public;
}
