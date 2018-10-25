#include <libgrimoire/security/cpkt.h>
#include <stdint.h>

#if 0
CPKT_INIT         (0)
#define CPKT_PROXY        (1)
#define CPKT_REKEY        (2)
#define CPKT_PLANE        (3)
#define CPKT_GET_SERVICES (4)
#endif

void cpkt_processing(cpkt_t * cpkt)
{
}

cpkt_t * create_cpkt(int type, int len)
{
	cpkt_t * cpkt = malloc(sizeof(cpkt_t)+len);
	int i;

	cpkt->type = type;

	for(i=0;i<sizeof(cpkt->iv);i++)
		cpkt->iv[i] = (uint8_t)rand();

	cpkt->payload_len = len;

	return cpkt;
}
