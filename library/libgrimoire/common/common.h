#ifndef _COMMON_
#define _COMMON_

#include <stdint.h>

#if 0
#define INITIALIZE	\
	__attribute__((constructor))

#define TERMINATION	\
	__attribute__((destructor))

#else
#define INITIALIZE

#define TERMINATION

#endif

void trimnl(char * str);
void binary_dump(char * title, uint8_t * buffer, int size);

typedef struct logger logger_t;
struct logger {
#define LOGMODE_ASYNC	(0)
#define LOGMODE_SYNC	(1)

#define LOG_ERROR	(0)
#define LOG_CRITICAL	(1)
#define LOG_WARNING	(2)
#define LOG_NOTICE	(3)
#define LOG_INFO	(4)
#define LOG_DEBUG	(5)
	int (*log)(logger_t *, int, const char *, ...);

	void (*set_level)(logger_t *, const char *);
	void (*set_mode)(logger_t *, const char *);
	void (*set_method)(logger_t *, const char *);
	void (*assign_logfile)(logger_t *, const char *);
};

logger_t * create_logger(void);

#endif
