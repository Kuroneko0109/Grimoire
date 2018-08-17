#ifndef _COMMON_
#define _COMMON_

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

#endif
