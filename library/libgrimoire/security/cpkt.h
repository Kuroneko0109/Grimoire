#ifndef _PROTOCOL_
#define _PROTOCOL_

#include <stdint.h>
#include <unistd.h>

#define CPKT_INIT         (0)
#define CPKT_PROXY        (1)
#define CPKT_REKEY        (2)
#define CPKT_PLANE        (3)
#define CPKT_GET_SERVICES (4)

typedef struct cpkt cpkt_t;
struct cpkt {
	int type;
	uint8_t iv[16];
	int payload_len;
	uint8_t payload[0];
};

/*
 * ------------------------
 * | cpkt | cpkt_init_hdr |
 * ------------------------
 */

typedef struct cpkt_init_hdr cpkt_init_hdr_t;
struct cpkt_init_hdr {
	int result;
	int sid;
};

/*
 * -------------------------
 * | cpkt | cpkt_proxy_hdr |
 * -------------------------
 */

typedef struct cpkt_proxy_hdr cpkt_proxy_hdr_t;
struct cpkt_proxy_hdr {
	int service_id;
	uint8_t payload[0];
};

/*
 * -------------------------
 * | cpkt | cpkt_rekey_hdr |
 * -------------------------
 */

typedef struct cpkt_rekey_hdr cpkt_rekey_hdr_t;
struct cpkt_rekey_hdr {
	uint8_t gx[256];
};

cpkt_t * create_cpkt(int id, int len);

#endif
