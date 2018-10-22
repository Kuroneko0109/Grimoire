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
	p1_init_t init_msg = {0, };
	p1_resp_t resp_msg = {0, };

	uint8_t hmac_id[64];
	int i;

	/* Random Key */
	srand(time(NULL));
	for(i=0;i<sizeof(init_msg.k);i++)
		init_msg.k[i] = (uint8_t)rand();

	strcpy(init_msg.id, priv->id);

	init_msg.group = 0;
#if 0
	printf("\nk : ");
	for(i=0;i<sizeof(init_msg.k);i++)
		printf("%02x", init_msg.k[i]);
#endif
	/* make hmac(k) */
	printf("\n");
	sa->sign(sa, init_msg.hmac_id, init_msg.id, sizeof(init_msg.id));
	printf("hmac(id) : ");
	for(i=0;i<sizeof(init_msg.hmac_id);i++)
		printf("%02x", init_msg.hmac_id[i]);
	printf("\n");

	peer->write(peer, &init_msg, sizeof(init_msg));

	peer->read(peer, &resp_msg, sizeof(resp_msg));	// get HMAC(K0, K)

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

void auth_client_set_psk(auth_client_t * this, char * id, uint8_t * psk)
{
	priv_auth_client_t * priv = (priv_auth_client_t *)this;
	sa_t * sa = priv->sa;

	priv->id = id;
	sa->set_akey(sa, psk, strlen(psk));
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
	public->contract_peer = auth_client_contract_peer;
	public->verify_peer = auth_client_verify_peer;

	private->dh->rand_init(private->dh);

	return public;
}
