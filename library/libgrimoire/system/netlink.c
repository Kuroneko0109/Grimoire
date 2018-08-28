#include <libgrimoire/system/netlink.h>

typedef struct priv_netlink priv_netlink_t;

struct priv_netlink {
	netlink_t public;

	int route_fd;

	struct sockaddr_nl nladdr;
};

int netlink_open(netlink_t * this)
{
	priv_netlink_t * priv = (priv_netlink_t *)this;
	if(private->route_fd == -1)
	{
		private->route_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

		bind(private->route_fd, (struct sockaddr *)&priv->nladdr, sizeof(priv->nladdr));
	}

	return private->route_fd;
}

void netlink_close(netlink_t * this)
{
	priv_netlink_t * priv = (priv_netlink_t *)this;
	if(private->route_fd != -1)
	{
		close(private->route_fd);
		private->route_fd = -1;
	}

	return private->route_fd;
}

priv_netlink_t * netlink_global;

void __attribute__((constructor)) init_netlink(void)
{
	netlink_global = malloc(sizeof(priv_netlink_t));

	priv_netlink_t * private = netlink_global;
	netlink_t * public = &private->public;

	private->nladdr.nl_family = AF_NETLINK;
	private->route_fd = -1;

	public->open = netlink_open;
}

netlink_t * get_netlink_global(void)
{
	return &netlink_global->public;
}
