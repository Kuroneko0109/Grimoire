#ifndef _ANALYZER_
#define _ANALYZER_

#include <stdint.h>

typedef struct analyzer analyzer_t;

struct analyzer {
	void (*input)(analyzer_t * this, uint8_t * pkt, int pktlen);
};

analyzer_t * create_analyzer(void);

#endif
