#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgrimoire/chrono/chrono.h>

typedef struct priv_chrono priv_chrono_t;

struct priv_chrono {
	chrono_t public;

	long long time_vector;
	long long time_limit;

	long long time_last;
};

void chrono_time_lapse(chrono_t * this, long long nanosec)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;

	priv->time_vector -= nanosec;
}

void chrono_time_slip(chrono_t * this, long long nanosec)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;

	priv->time_vector += nanosec;
}

void chrono_set_period(chrono_t * this, long long nanosec)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;

	priv->time_limit = nanosec;
}

void chrono_dump(chrono_t * this)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;

	printf("vector : %lld, limit : %lld, last : %lld\n", priv->time_vector, priv->time_limit, priv->time_last);
}

int chrono_check_period(chrono_t * this)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;
	int ret;

	long long now;
	long long diff;

	ret = 0;

	now = get_nanosec();
	diff = now - priv->time_last;

	this->time_slip(this, diff);

	if(priv->time_limit < priv->time_vector)
		ret = 1;

	return ret;
}

int chrono_check_period_reset(chrono_t * this)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;
	int ret;

	long long now;
	long long diff;

	ret = 0;

	now = get_nanosec();
	diff = now - priv->time_last;

	this->time_slip(this, diff);

	if(priv->time_limit < priv->time_vector)
	{
		this->time_lapse(this, priv->time_limit);
		ret = 1;
	}

/*
	if(ret)
		this->dump(this);
*/
	this->start(this);

	return ret;
}

void chrono_start(chrono_t * this)
{
	priv_chrono_t * priv = (priv_chrono_t *)this;

	priv->time_last = get_nanosec();
	priv->time_vector = 0;
}

void chrono_destroy(chrono_t * this)
{
	free(this);
}

chrono_t * create_chrono(void)
{
	priv_chrono_t * private;
	chrono_t * public;

	private = malloc(sizeof(priv_chrono_t));
	public = &private->public;

	public->time_lapse = chrono_time_lapse;
	public->time_slip = chrono_time_slip;
	public->set_period = chrono_set_period;
	public->check_period = chrono_check_period;
	public->check_period_reset = chrono_check_period_reset;

	public->start = chrono_start;

	public->dump = chrono_dump;
	public->destroy = chrono_destroy;

	private->time_vector = 0;
	private->time_limit = 0;

	return public;
}
