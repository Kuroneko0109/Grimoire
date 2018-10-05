#include <openssl/ssl.h>

#include <libgrimoire/grid/peer.h>
#include <libgrimoire/grid/acceptor.h>
#include <libgrimoire/security/tls.h>

void init_tls(void)
{
	static int is_init = 0;

	if(0 == is_init)
	{
		SSL_library_init();
		SSL_load_error_strings();
		ERR_load_BIO_strings();
	}
}
