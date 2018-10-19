#include <libgrimoire/security/auth_client.h>
//#include <auth_client/auth_client.h>
#include <stdlib.h>
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
	uint8_t * psk;
};

int auth_client_contract_peer(auth_client_t * this, peer_t * peer)
{
	uint8_t buffer[1024];
	uint8_t rnd[64];	// 512bit = 
	int fd;
        fd = open("/dev/random", O_RDONLY);
        read(fd, &rnd, 64);
        close(fd);

	peer->write(peer, rnd, sizeof(rnd));	// send K
	peer->read(peer, buffer, sizeof(buffer));	// get HMAC(K0, K)

	return this->verify_peer(this, peer);
}

int auth_client_verify_peer(auth_client_t * this, peer_t * peer)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	dh_t * dh = priv->dh;
	uint8_t buffer[1024];

	size_t gx_size;
	uint8_t * gx_modp = NULL;

	dh->rand(dh);
	dh->g_x_mod(dh);

	gx_modp = (uint8_t *)dh->export(dh, &gx_size);

	peer->write(peer, gx_modp, gx_size);

	peer->read(peer, buffer, sizeof(buffer));

	dh->g_xy_mod(dh, buffer);

	dh->dump(dh);

	if(gx_modp)
		free(gx_modp);

	return 0;
}

void auth_client_set_psk(auth_client_t * this, uint8_t * psk)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	priv->psk = psk;
}

auth_client_t * create_auth_client(void)
{
	priv_auth_client_t * private;
	auth_client_t * public;

	private = malloc(sizeof(priv_auth_client_t));
	public = &private->public;

	private->dh = create_dh(5);
	public->set_psk = auth_client_set_psk;
	public->contract_peer = auth_client_contract_peer;
	public->verify_peer = auth_client_verify_peer;

	private->dh->rand_init(private->dh);

	return public;
}
