#include <libgrimoire/grid/stream.h>
#include <libgrimoire/chrono/chrono.h>

#define STREAM_RW_DONE	0
#define STREAM_RW_INIT	1
#define STREAM_RW_MORE	2
#define STREAM_RW_FAIL	3

typedef struct priv_stream priv_stream_t;

struct priv_stream {
	stream_t public;

	chrono_t * chrono;

	peer_t * peer;

	int read_idx;
	size_t read_size;
	void * read_dst;
	int write_idx;
	size_t write_size;
	void * write_src;

	int status;
};

void stream_set_read_info(stream_t * this, void * dst, size_t size)
{
	priv_stream_t * priv = (priv_stream_t *)this;
	priv->read_size = size;
	priv->read_dst = dst;
	priv->read_idx = 0;
}

ssize_t stream_read(stream_t * this)
{
	priv_stream_t * priv = (priv_stream_t *)this;
	peer_t * peer = priv->peer;
	ssize_t rc;

	rc = peer->read(peer, &((uint8_t *)(priv->read_dst))[priv->read_idx], priv->read_size);

	priv->read_idx += rc;
	priv->read_size -= rc;

	return priv->status;
}

void stream_set_write_info(stream_t * this, void * src, size_t size)
{
	priv_stream_t * priv = (priv_stream_t *)this;
	priv->write_size = size;
	priv->write_src = src;
	priv->write_idx = 0;
}

ssize_t stream_write(stream_t * this)
{
	priv_stream_t * priv = (priv_stream_t *)this;
	peer_t * peer = priv->peer;
	ssize_t wc;

	wc = peer->write(peer, &((uint8_t *)(priv->write_src))[priv->write_idx], priv->write_size);

	priv->write_idx += wc;
	priv->write_size -= wc;

	return priv->status;
}

void stream_set_sequence_timeout(stream_t * this)
{
}

void stream_destroy(stream_t * this)
{
	priv_stream_t * priv = (priv_stream_t *)this;

	priv->chrono->destroy(priv->chrono);
	priv->peer->destroy(priv->peer);

	free(this);
}

stream_t * create_stream(peer_t * peer)
{
	priv_stream_t * private;
	stream_t * public;

	if(NULL == peer)
		return NULL;

	private = malloc(sizeof(priv_stream_t));
	public = &private->public;

	private->peer = peer;
	private->status = STREAM_RW_INIT;

	public->set_read_info = stream_set_read_info;
	public->read = stream_read;
	public->set_write_info = stream_set_write_info;
	public->write = stream_write;

	public->set_sequence_timeout = stream_set_sequence_timeout;
	public->destroy = stream_destroy;

	return public;
}
