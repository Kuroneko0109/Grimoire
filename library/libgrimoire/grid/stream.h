#ifndef _STREAM_
#define _STREAM_

#include <stdlib.h>
#include <libgrimoire/grid/peer.h>

typedef struct stream stream_t;

struct stream {
	void (*set_read_info)(stream_t * this, void * dst, size_t size);
	ssize_t (*read)(stream_t * this);
	void (*set_write_info)(stream_t * this, void * src, size_t size);
	ssize_t (*write)(stream_t * this);

	void (*set_sequence_timeout)(stream_t * this);

	void (*destroy)(stream_t * this);
};

stream_t * create_stream(peer_t * peer);

#endif
