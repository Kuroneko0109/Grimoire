#include <libgrimoire/network/pkt_collector.h>
#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/datastructure/iterator.h>

#include <stdint.h>
#include <pcap.h>

#define MAX_L2_PAYLOAD	(65536)

typedef struct priv_pkt_collector priv_pkt_collector_t;

struct priv_pkt_collector {
	pkt_collector_t public;

	list_t * device;
	iterator_t * iterator;
	uint8_t buffer[MAX_L2_PAYLOAD];

	pcap_t * pcap;
};

int pkt_collector_init(pkt_collector_t * this)
{
	priv_pkt_collector_t * priv = (priv_pkt_collector_t *)this;
	list_t * device = priv->device;
	struct bpf_program fp;

	char errbuf[1024];
	int ret;

	char * dev;

	priv->iterator = device->get_iterator(device);
	dev = priv->iterator->next(priv->iterator);

	priv->pcap = pcap_open_live(dev, sizeof(priv->buffer), 1, 1000, errbuf);
	printf("%s\n", errbuf);

	ret = pcap_compile(priv->pcap, &fp, NULL, 0, 0);
	pcap_datalink(priv->pcap);
}

int pkt_collector_gather(pkt_collector_t * this)
{
	priv_pkt_collector_t * priv = (priv_pkt_collector_t *)this;
	uint8_t * pkt;
	struct pcap_pkthdr hdr;

	pkt = pcap_next(priv->pcap, &hdr);
	if(pkt)
		memcpy(priv->buffer, pkt, hdr.len);

	return hdr.len;
}

void pkt_collector_watch_device(pkt_collector_t * this, char * devname)
{
	priv_pkt_collector_t * priv = (priv_pkt_collector_t *)this;
	list_t * device = priv->device;

	device->enqueue_data(device, devname);
}

pkt_collector_t * create_pkt_collector(void)
{
	priv_pkt_collector_t * private;
	pkt_collector_t * public;

	private = malloc(sizeof(priv_pkt_collector_t));
	public = &private->public;

	private->device = create_list(NULL, NULL, NULL);

	public->gather = pkt_collector_gather;
	public->watch_device = pkt_collector_watch_device;
	public->init = pkt_collector_init;

	return public;
}
