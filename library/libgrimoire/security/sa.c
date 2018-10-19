#include <libgrimoire/security/sa.h>
#include <stdlib.h>

#include <stdint.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

typedef struct cipher_info cipher_info_t;

typedef struct priv_sa priv_sa_t;

struct priv_sa {
	sa_t public;

	EVP_CIPHER_CTX * ectx;
	uint8_t iv[32];
	uint8_t ekey[32];

	uint8_t akey[128];
};

void sa_set_ekey(sa_t * this, uint8_t * iv, int ivlen, uint8_t * key, int klen)
{
	priv_sa_t * priv = (priv_sa_t *)this;
	memcpy(priv->iv, iv, ivlen);
	memcpy(priv->ekey, key, klen);
}

int sa_encrypt(sa_t * this, void * dst, void * src, int len)
{
	priv_sa_t * priv = (priv_sa_t *)this;
	int outlen;
	int tmp;

	EVP_EncryptInit_ex(priv->ectx, EVP_aes_256_cbc(), NULL, priv->ekey, priv->iv);

	EVP_EncryptUpdate(priv->ectx, dst, &tmp, src, len);

	EVP_EncryptFinal_ex(priv->ectx, dst + tmp, &outlen);

	return outlen;
}

int sa_decrypt(sa_t * this, void * dst, void * src, int len)
{
	priv_sa_t * priv = (priv_sa_t *)this;
	int outlen;
	int tmp;

	EVP_DecryptInit_ex(priv->ectx, EVP_aes_256_cbc(), NULL, priv->ekey, priv->iv);

	EVP_DecryptUpdate(priv->ectx, dst, &tmp, src, len);

	EVP_DecryptFinal_ex(priv->ectx, dst + tmp, &outlen);

	return outlen;
}

void sa_set_akey(sa_t * this, uint8_t * key, int klen)
{
	priv_sa_t * priv = (priv_sa_t *)this;

	memcpy(priv->akey, key, klen);
}

void sa_sign(sa_t * this, void * dst, void * src, int len)
{
	priv_sa_t * priv = (priv_sa_t *)this;
	int digest_len;
	uint8_t * hmac;

	printf("%s(%d)\n", __func__, __LINE__);
	hmac = HMAC(EVP_sha256(), priv->akey, 32, src, len, NULL, NULL);

	int i;
	printf("HMAC : ");
	for(i=0;i<32;i++)
		printf("%02x", hmac[i]);
	printf("\n");

	return digest_len;
}

void sa_destroy(sa_t * this)
{
	priv_sa_t * priv = (priv_sa_t *)this;

	EVP_CIPHER_CTX_free(priv->ectx);

	free(this);
}

sa_t * create_sa(void)
{
	priv_sa_t * private;
	sa_t * public;

	private = malloc(sizeof(priv_sa_t));
	public = &private->public;

	private->ectx = EVP_CIPHER_CTX_new();

	public->set_ekey = sa_set_ekey;
	public->encrypt = sa_encrypt;
	public->decrypt = sa_decrypt;

	public->set_akey = sa_set_akey;
	public->sign = sa_sign;

	return public;
}