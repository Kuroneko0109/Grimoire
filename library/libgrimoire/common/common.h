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

#endif
