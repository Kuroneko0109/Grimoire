#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <libgrimoire/grid/acceptor.h>
#include <libgrimoire/datastructure/list.h>

acceptor_t * create_acceptor(int af)
{
	acceptor_t * acceptor = NULL;

	switch(af)
	{
		case AF_UNIX	:
			acceptor = create_acceptor_uds();
			break;
		case AF_INET	:
			acceptor = create_acceptor_tcp();
			break;
		default	:
			break;
	}

	return acceptor;
}
