#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <libgrimoire/grid/peer.h>

peer_t * create_peer(int af)
{
	peer_t * peer = NULL;
	switch(af)
	{
		case AF_UNIX	:
			peer = create_peer_uds();
			break;
		case AF_INET	:
			peer = create_peer_tcp();
			break;
		default	:
			peer = NULL;
			break;
	}

	return peer;
}
