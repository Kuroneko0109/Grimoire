#include <libgrimoire/security/auth_server.h>
//#include <auth_server/auth_server.h>
#include <stdlib.h>
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
	uint8_t * psk;
};

int auth_server_contract_peer(auth_server_t * this, peer_t * peer)
{
	uint64_t k;

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

void auth_server_set_psk(auth_server_t * this, uint8_t * psk)
{
	priv_auth_server_t * priv = (priv_auth_server_t *)this;
	priv->psk = psk;
}

auth_server_t * create_auth_server(void)
{
	priv_auth_server_t * private;
	auth_server_t * public;

	private = malloc(sizeof(priv_auth_server_t));
	public = &private->public;

	private->dh = create_dh(5);
	public->set_psk = auth_server_set_psk;
	public->verify_peer = auth_server_verify_peer;
	public->contract_peer = auth_server_contract_peer;

	private->dh->rand_init(private->dh);

	return public;
}
