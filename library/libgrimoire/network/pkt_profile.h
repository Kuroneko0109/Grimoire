#ifndef _PKT_PROFILE_
#define _PKT_PROFILE_

#include <stdint.h>
#include <pcap.h>

typedef struct pkt_profile pkt_profile_t;

struct pkt_profile {
	struct timeval ts;
	uint32_t caplen;
	uint32_t len;

	uint8_t * pkt;

	char * device;
};

pkt_profile_t * create_pkt_profile(uint8_t *, struct pcap_pkthdr *, const char *);

#endif
