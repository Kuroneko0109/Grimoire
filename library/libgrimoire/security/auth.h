#ifndef _AUTH_
#define _AUTH_

#include <libgrimoire/security/dh.h>
#include <libgrimoire/grid/peer.h>
#include <stdint.h>

typedef struct auth auth_t;

struct auth {
	int (*verify_peer)(auth_t * auth, peer_t * peer);
	void (*set_psk)(auth_t * auth, uint8_t * psk);
};

auth_t * create_auth(void);

#endif
