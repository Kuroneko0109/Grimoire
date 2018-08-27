#include <stdlib.h>

#include <libgrimoire/datastructure/node.h>

typedef struct priv_node priv_node_t;

struct priv_node {
	node_t public;
	void * data;

	node_t * front;
	node_t * rear;

	void * (*destroyer)(void *);
};

void node_set_data(node_t * this, void * data)
{
	priv_node_t * priv = (priv_node_t *)this;

	priv->data = data;
}

void * node_get_data(node_t * this)
{
	priv_node_t * priv = (priv_node_t *)this;
	void * data;

	data = priv->data;

	return data;
}

node_t * node_get_front(node_t * this)
{
	priv_node_t * priv = (priv_node_t *)this;
	return priv->front;
}

node_t * node_set_front(node_t * this, node_t * node)
{
	priv_node_t * priv = (priv_node_t *)this;
	node_t * past;

	past = priv->front;
	priv->front = node;

	return past;
}

node_t * node_get_rear(node_t * this)
{
	return ((priv_node_t *)this)->rear;
}

node_t * node_set_rear(node_t * this, node_t * node)
{
	priv_node_t * priv = (priv_node_t *)this;
	node_t * past;

	past = priv->rear;
	priv->rear = node;

	return past;
}

void node_destroy(node_t * this)
{
	priv_node_t * priv = (priv_node_t *)this;

	if(priv->data)
	{
		if(priv->destroyer)
			priv->destroyer(priv->data);
		else
			free(priv->data);
	}

	free(this);
}

void node_swap(node_t * this, node_t * dst)
{
	priv_node_t * priv_this = (priv_node_t *)this;
	priv_node_t * priv_dst = (priv_node_t *)dst;

	priv_node_t tmp;

	tmp.data = priv_this->data;
	tmp.destroyer = priv_this->destroyer;

	priv_this->data = priv_dst->data;
	priv_this->destroyer = priv_dst->destroyer;

	priv_dst->data = tmp.data;
	priv_dst->destroyer = tmp.destroyer;
}

node_t * create_node(void * data, void * (*destroyer)(void *))
{
	priv_node_t * priv;
	node_t * public;

	priv = malloc(sizeof(priv_node_t));
	public = &priv->public;

	public->set_data = node_set_data;
	public->get_data = node_get_data;
	public->get_front = node_get_front;
	public->get_rear = node_get_rear;
	public->set_front = node_set_front;
	public->set_rear = node_set_rear;
	public->swap = node_swap;
	public->destroy = node_destroy;

	priv->front = NULL;
	priv->rear = NULL;
	priv->data = data;
	priv->destroyer = destroyer;

	return public;
}
