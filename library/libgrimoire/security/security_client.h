#ifndef _SECURITY_CLIENT_
#define _SECURITY_CLIENT_

#include <stdlib.h>
#include <libgrimoire/grid/peer.h>
#include <libgrimoire/security/sa.h>

#define SPEER_MODE_SERVER (0)
#define SPEER_MODE_CLIENT (1)

typedef struct security_client security_client_t;

struct security_client {
	size_t (*write)(security_client_t * this, int id, void * src, size_t size);
	size_t (*read)(security_client_t * this, void * dst, size_t size);
	void (*rekey)(security_client_t * this);
	void (*destroy)(security_client_t * this);
};

security_client_t * create_security_client(int mode, peer_t * peer, sa_t * sa);

#endif
