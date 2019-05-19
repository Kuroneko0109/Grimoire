#ifndef _ETHER_ANALYSIS_
#define _ETHER_ANALYSIS_

typedef struct ether_analysis ether_analysis_t;

struct ether_analysis {
	void (*input)(ether_analysis_t *, void *);
	int (*check_protocol)(ether_analysis_t *, void *);
};

ether_analysis_t * create_ether_analysis(void);

#endif
