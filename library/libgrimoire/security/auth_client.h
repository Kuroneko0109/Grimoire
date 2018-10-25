#ifndef _AUTH_CLIENT_
#define _AUTH_CLIENT_

#include <libgrimoire/security/dh.h>
#include <libgrimoire/security/security_client.h>
#include <libgrimoire/grid/peer.h>
#include <stdint.h>

typedef struct auth_client auth_client_t;

struct auth_client {
	security_client_t * (*contract_peer)(auth_client_t * this, peer_t * peer);
	void (*set_psk)(auth_client_t * this, char * id, uint8_t * psk);
        void (*set_ekey)(auth_client_t * this, uint8_t * key, int klen);
        void (*set_akey)(auth_client_t * this, uint8_t * key, int klen);
};

auth_client_t * create_auth_client(int type);

#endif
