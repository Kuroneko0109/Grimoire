#ifndef _CHRONO_
#define _CHRONO_

#include <time.h>
#include <stdint.h>

#define SEC_TO_NANOSEC	(1000000000ULL)

static inline uint64_t get_nanosec(void)
{
	struct timespec tv = {0, };
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return ((tv.tv_sec * SEC_TO_NANOSEC) + tv.tv_nsec);
}

typedef struct chrono chrono_t;

struct chrono {
	void (*time_lapse)(chrono_t * this, uint64_t nanosec);
	void (*time_slip)(chrono_t * this, uint64_t nanosec);
	void (*set_period)(chrono_t * this, uint64_t nanosec);

	int (*check_period)(chrono_t * this);
	int (*check_period_reset)(chrono_t * this);

	void (*start)(chrono_t * this);

	void (*dump)(chrono_t * this);
	void (*destroy)(chrono_t * this);
};

chrono_t * create_chrono(void);

#endif
