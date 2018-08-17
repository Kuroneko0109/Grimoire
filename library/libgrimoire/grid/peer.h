#ifndef _PEER_
#define _PEER_

#include <sys/socket.h>
#include <stdint.h>

#define PEER_UDS	(AF_UNIX)
#define PEER_INET	(AF_INET)

typedef struct peer peer_t;

struct peer {
	void (*set_fd)(peer_t * this, int fd);
	int (*open)(peer_t * this);
	void (*close)(peer_t * this);
	ssize_t (*read)(peer_t * this, void * dst, size_t size);
	ssize_t (*write)(peer_t * this, void * src, size_t size);

	void (*set_addr)(peer_t * this, const char * addr_str);
	void (*set_port)(peer_t * this, uint16_t port);
	void (*set_blk)(peer_t * this);
	void (*set_nblk)(peer_t * this);
	void (*dump)(peer_t * this);
	void (*destroy)(peer_t * this);

	void (*mode_stream)(peer_t * this);
};
// client_addr needed

peer_t * create_peer(int af);
peer_t * create_peer_tcp(void);
peer_t * create_peer_uds(void);

#endif
