#ifndef _SECURITY_SERVER_
#define _SECURITY_SERVER_

#include <stdlib.h>
#include <libgrimoire/grid/peer.h>
#include <libgrimoire/security/sa.h>

#define SPEER_MODE_SERVER (0)
#define SPEER_MODE_CLIENT (1)

typedef struct security_server security_server_t;

struct security_server {
	size_t (*write)(security_server_t * this, int id, void * src, size_t size);
	size_t (*read)(security_server_t * this, void * dst, size_t size);
	void (*rekey)(security_server_t * this);
	void (*destroy)(security_server_t * this);
};

security_server_t * create_security_server(int mode, peer_t * peer, sa_t * sa);

#endif
