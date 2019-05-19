#include <libgrimoire/network/analyzer/ether.h>
#include <netinet/ether.h>

#include <stdlib.h>
#include <stdio.h>

typedef struct priv_ether_analysis priv_ether_analysis_t;
struct priv_ether_analysis {
	ether_analysis_t public;
};

void ether_analysis_input(ether_analysis_t * this, void * data)
{
	struct ether_header * hdr = data;

	this->check_protocol(this, data);
}

int ether_analysis_check_protocol(ether_analysis_t * this, void * data)
{
	struct ether_header * hdr = data;

	switch(hdr->ether_type)
	{
		case ETHERTYPE_IP :
		case ETHERTYPE_ARP :
		case ETHERTYPE_IPV6 :
		case ETHERTYPE_REVARP :
			return 0;
		default :
			printf("%s(%d) Protocol not supported(%d)\n",
					__func__, __LINE__, (int)hdr->ether_type);
			return -1;
	}
}

ether_analysis_t * create_ether_analysis(void)
{
	priv_ether_analysis_t * private;
	ether_analysis_t * public;

	private = malloc(sizeof(priv_ether_analysis_t));
	public = &private->public;

	public->input = ether_analysis_input;
	public->check_protocol = ether_analysis_check_protocol;

	return public;
}
