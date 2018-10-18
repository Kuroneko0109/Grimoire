#include <libgrimoire/security/auth.h>
//#include <auth/auth.h>
#include <stdlib.h>
#include <stdio.h>

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

typedef struct priv_auth priv_auth_t;

struct priv_auth {
	auth_t public;
	
	dh_t * dh;
	uint8_t * psk;
};

int auth_verify_peer(auth_t * this, peer_t * peer)
{
	return 0;
	priv_auth_t * priv = (priv_auth_t *)this;
	dh_t * dh = priv->dh;

	size_t gx_size;
	uint8_t * gx_modp;

	dh->rand(dh);
	dh->g_x_mod(dh);
	dh->get_g_x_mod(dh);

	gx_modp = (uint8_t *)dh->export(dh, &gx_size);

	int i;
	for(i=0;i<gx_size;i++)
		printf("%02x", gx_modp[i]);
	printf("\n");

	free(gx_modp);

	return 0;
}

void auth_set_psk(auth_t * this, uint8_t * psk)
{
	priv_auth_t * priv = (priv_auth_t *)this;
	priv->psk = psk;
}

auth_t * create_auth(void)
{
	priv_auth_t * private;
	auth_t * public;

	private = malloc(sizeof(priv_auth_t));
	public = &private->public;

	private->dh = create_dh(14);
	public->set_psk = auth_set_psk;
	public->verify_peer = auth_verify_peer;

	return public;
}
