#ifndef _SA_
#define _SA_

#include <stdint.h>

typedef struct sa sa_t;

struct sa {
	void (*gen_iv)(sa_t * this);
	void (*set_iv)(sa_t * this, uint8_t * src);
	void (*get_iv)(sa_t * this, uint8_t * dst);
	void (*set_ekey)(sa_t * this, uint8_t * key, int klen);
	int (*encrypt)(sa_t * this, void * dst, void * src, int len);
	int (*decrypt)(sa_t * this, void * dst, void * src, int len);

	void (*set_akey)(sa_t * this, uint8_t * key, int klen);
	int (*sign)(sa_t * this, void * dst, void * src, int len);

	void (*destroy)(sa_t * this);
};

sa_t * create_sa(void);

#endif
