#ifndef _PKT_COLLECTOR_
#define _PKT_COLLECTOR_

typedef struct pkt_collector pkt_collector_t;

struct pkt_collector {
	int (*gather)(pkt_collector_t *);
	void (*watch_device)(pkt_collector_t *, char *);
	int (*init)(pkt_collector_t *);
};

pkt_collector_t * create_pkt_collector(void);

#endif
