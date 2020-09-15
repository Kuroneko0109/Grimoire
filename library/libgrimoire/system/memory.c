#include <libgrimoire/system/memory.h>

void * galloc(size_t size)
{
#ifdef __KERNEL__
	return kmalloc(size, GFP_ATOMIC);
#else
	return malloc(size);
#endif
}
