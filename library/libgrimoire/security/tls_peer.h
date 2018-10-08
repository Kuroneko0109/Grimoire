#ifndef _TLS_PEER_
#define _TLS_PEER_

#include <openssl/ssl.h>

typedef struct tls_peer tls_peer_t;

struct tls_peer {
	int (*connect)(tls_peer_t * this);

	int (*read)(tls_peer_t * this, void * dst, int len);
	int (*write)(tls_peer_t * this, const void * src, int len);

	void (*set_addr)(tls_peer_t * this, const char * addr);
	void (*set_port)(tls_peer_t * this, uint16_t port);

	void (*destroy)(tls_peer_t * this);
};

tls_peer_t * create_tls_peer(SSL * ssl);

#endif
