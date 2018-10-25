#ifndef _AUTH_SERVER_
#define _AUTH_SERVER_

#include <libgrimoire/security/dh.h>
#include <libgrimoire/security/security_server.h>
#include <libgrimoire/security/sa.h>
#include <libgrimoire/grid/peer.h>
#include <stdint.h>

typedef struct auth_server auth_server_t;

struct auth_server {
	security_server_t * (*contract_peer)(auth_server_t * this, peer_t * peer);
	void (*set_akey_by_psk)(auth_server_t * this, sa_t * sa, uint8_t * id);
	void (*set_ekey)(auth_server_t * this, uint8_t * key, int klen);
	void (*set_akey)(auth_server_t * this, uint8_t * key, int klen);
};

auth_server_t * create_auth_server(char * account_path);

#endif
