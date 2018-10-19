#ifndef _CIPHER_
#define _CIPHER_

#include <stdint.h>

typedef struct cipher cipher_t;

struct cipher {
	int (*register_sa)(cipher_t * this, const char * type, const char * name);	// returns session id
	void (*set_key)(cipher_t * this, int session_id, int klen, uint8_t * key);
};

cipher_t * create_cipher(void);

#endif
