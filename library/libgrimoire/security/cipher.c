#include <libgrimoire/security/cipher.h>
#include <libgrimoire/datastructure/list.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/err.h>

typedef struct priv_cipher priv_cipher_t;

struct priv_cipher {
	cipher_t public;
	int last_id;

	list_t * sa_list;
};

int cipher_register_sa(cipher_t * this, const char * type, const char * name)
{
	priv_cipher_t * priv = (priv_cipher_t *)this;
}

void cipher_set_key(cipher_t * this, int session_id, int klen, uint8_t * key)
{
	priv_cipher_t * priv = (priv_cipher_t *)this;
}

int cipher_session_id_compare(void * _s, void * _d)
{
//	sa_t * s = _s;
//	sa_t * d = _d;

//	return s->id - d->id;
	return 0;
}

cipher_t * create_cipher(void)
{
	priv_cipher_t * private;
	cipher_t * public;

	private = malloc(sizeof(priv_cipher_t));
	public = &private->public;

	private->last_id = 0;
	private->sa_list = create_list(NULL, cipher_session_id_compare, NULL);

	public->register_sa = cipher_register_sa;
	public->set_key = cipher_set_key;

	return public;
}

void __attribute__((constructor)) cipher_init(void)
{
//	OpenSSL_init_crypto();
//	OpenSSL_init_ssl();
	OpenSSL_add_all_algorithms();
//	SSL_load_error_strings();
}
