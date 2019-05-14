#ifndef _PKT_PROFILE_
#define _PKT_PROFILE_

typedef struct pkt_profile pkt_profile_t;

struct pkt_profile {
	uint8_t * pkt;
	int pkt_len;

	char * device;
};

pkt_profile_t * pkt_profile_create(uint8_t *, int, char *);

#endif
