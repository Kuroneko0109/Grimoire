#ifndef _NETLINK_
#define _NETLINK_

typedef struct netlink netlink_t;

struct netlink {
	int (*open)(netlink_t * this);
	void (*close)(netlink_t * this);
	int (*recv)(netlink_t * this);
};

netlink_t * get_netlink_global(void);

#endif
