#include <libgrimoire/network/pkt_profile.h>
#include <stdlib.h>

pkt_profile_t * create_pkt_profile(uint8_t * pkt, struct pcap_pkthdr * phdr, const char * device)
{
	pkt_profile_t * this = malloc(sizeof(pkt_profile_t));
	const char * undefined = "Undefined";

	memcpy(&this->ts, &phdr->ts, sizeof(this->ts));
	this->len = phdr->len;
	this->caplen = phdr->caplen;

	this->pkt = malloc(this->len);
	memcpy(this->pkt, pkt, this->len);

	if(device)
	{
		this->device = malloc(strlen(device));
		strcpy(this->device, device);
	}
	else
	{
		this->device = malloc(strlen(undefined));
		strcpy(this->device, undefined);
	}

	return this;
}
