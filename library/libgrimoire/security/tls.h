#ifndef _TLS_
#define _TLS_

#include <libgrimoire/config/config.h>

typedef struct tls tls_t;

struct tls {
	void (*init)(tls_t * this, config_t * config);
};

tls_t * create_tls(void);

void init_tls(void);

#endif
