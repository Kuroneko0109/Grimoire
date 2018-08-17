#ifndef _LISTENER_
#define _LISTENER_

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <libgrimoire/grid/peer.h>

typedef struct acceptor acceptor_t;

struct acceptor {
	void (*set_addr)(acceptor_t * this, const char * addr);
	void (*set_port)(acceptor_t * this, uint16_t port);
	int (*open)(acceptor_t * this);
	int (*accept)(acceptor_t * this);
	peer_t * (*accept_peer)(acceptor_t * this);
	void (*close)(acceptor_t * this);
	void (*destroy)(acceptor_t * this);
};

acceptor_t * create_acceptor(int af);
acceptor_t * create_acceptor_tcp(void);
acceptor_t * create_acceptor_uds(void);

#endif
