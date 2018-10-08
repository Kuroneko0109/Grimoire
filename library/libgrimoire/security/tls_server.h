#ifndef _TLS_SERVER_
#define _TLS_SERVER_

#include <stdint.h>
#include <libgrimoire/config/config.h>
#include <libgrimoire/security/tls_peer.h>

typedef struct tls_server tls_server_t;

struct tls_server {
	tls_peer_t * (*accept)(tls_server_t * this);

	int (*open)(tls_server_t * this);
	void (*close)(tls_server_t * this);

	void (*set_addr)(tls_server_t * this, const char * addr);
	void (*set_port)(tls_server_t * this, uint16_t port);
};

tls_server_t * create_tls_server(config_t * config);

#endif
