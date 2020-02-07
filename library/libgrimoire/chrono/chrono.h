#ifndef _CHRONO_
#define _CHRONO_

#include <time.h>

#define SEC_TO_NANOSEC	(1000000000LL)

static inline long long get_nanosec(void)
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return ((tv.tv_sec * SEC_TO_NANOSEC) + tv.tv_nsec);
}

typedef struct chrono chrono_t;

struct chrono {
	void (*time_lapse)(chrono_t * this, long long nanosec);
	void (*time_slip)(chrono_t * this, long long nanosec);
	void (*set_period)(chrono_t * this, long long nanosec);

	int (*check_period)(chrono_t * this);
	int (*check_period_reset)(chrono_t * this);

	void (*start)(chrono_t * this);

	void (*dump)(chrono_t * this);
	void (*destroy)(chrono_t * this);
};

chrono_t * create_chrono(void);

#endif
