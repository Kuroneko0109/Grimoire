#ifndef _AUTH_SERVER_
#define _AUTH_SERVER_

#include <libgrimoire/security/dh.h>
#include <libgrimoire/grid/peer.h>
#include <stdint.h>

typedef struct auth_server auth_server_t;

struct auth_server {
	int (*contract_peer)(auth_server_t * this, peer_t * peer);
	int (*verify_peer)(auth_server_t * this, peer_t * peer);
	void (*set_psk)(auth_server_t * this, uint8_t * psk);
};

auth_server_t * create_auth_server(char * account_path);

#endif
