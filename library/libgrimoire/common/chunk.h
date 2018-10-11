#ifndef _CHUNK_
#define _CHUNK_

#include <stdint.h>

typedef struct chunk chunk_t;

struct chunk {
	int (*get_size)(chunk_t * this);
	uint8_t * (*get_data)(chunk_t * this);
	void (*destroy)(chunk_t * this);
};

chunk_t * create_chunk(int size, void * data);

#endif
