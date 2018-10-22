#ifndef _AUTH_
#define _AUTH_

#include <stdint.h>

typedef struct p1_init p1_init_t;

struct p1_init {
	char id[32];
	uint8_t hmac_id[64];
	uint8_t k[64];
	int group;
};

typedef struct p1_resp p1_resp_t;

struct p1_resp {
	uint8_t hmac_k[64];
	uint8_t modp[256];
	uint8_t hmac_modp[64];
};

typedef struct p2_init p2_init_t;

struct p2_init {
	uint8_t modp[256];
	uint8_t hmac_modp[64];
	int service_id;
};

typedef struct p2_resp p2_resp_t;

struct p2_resp {
	int result;
	int sid;
};

#endif
