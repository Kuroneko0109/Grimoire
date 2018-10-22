#ifndef _AUTH_CLIENT_
#define _AUTH_CLIENT_

#include <libgrimoire/security/dh.h>
#include <libgrimoire/grid/peer.h>
#include <stdint.h>

typedef struct auth_client auth_client_t;

struct auth_client {
	int (*accept_peer)(auth_client_t * this, peer_t * peer);
	int (*contract_peer)(auth_client_t * this, peer_t * peer);
	int (*verify_peer)(auth_client_t * this, peer_t * peer);
	void (*set_psk)(auth_client_t * this, char * id, uint8_t * psk);
};

auth_client_t * create_auth_client(void);

#endif
