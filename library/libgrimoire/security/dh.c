#include <gmp.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgrimoire/system/memory.h>
#include <libgrimoire/security/dh.h>

/*
 * Alice : Select Primary(p), Select Generator(g), Select random EXP(ea)
 * Bob : Select Primary(p), Select Generator(g), Select random EXP(eb)
 * 
 * DH Group 14. Shared Generator, Primary.
 *
 * Alice : a = g^ea mod p, send this value to bob
 * Bob : b = g^eb mod p, send this value to alice.
 *
 * Alice : Calculate b^ea mod p.
 * Bob : Calculate a^eb mod p.
 */

typedef struct dh_param dh_param_t;

struct dh_param {
	char * modp_str;
	int bits;
	int bytes;
	int supported;
};

const dh_param_t dh_param[32] = {
	[5] = {
		.modp_str = MODP1536_STR,
		.bits = 1536,
		.bytes = 1536/8,
		.supported = 1,
	},
	[14] = {
		.modp_str = MODP2048_STR,
		.bits = 2048,
		.bytes = 2048/8,
		.supported = 1,
	},
};

typedef struct priv_dh priv_dh_t;

struct priv_dh {
	dh_t public;

	int group;
	dh_param_t * group_info;

	mpz_t prime;
	mpz_t generator;

	mpz_t exp;

	/* RNG */
	gmp_randstate_t rng_stat;

	mpz_t gx_modp;
	mpz_t gxy_modp;
};

void dh_set_group(dh_t * this, int group)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	dh_param_t * group_info = &priv->group_info[group];

	if(group_info->supported == 0)
	{
		printf("Group %d is not supported\n", group);
		return;
	}

	mpz_clear(priv->generator);
	mpz_clear(priv->prime);
	mpz_clear(priv->exp);
	mpz_init(priv->generator);
	mpz_init(priv->prime);
	mpz_init(priv->exp);

	mpz_set_ui(priv->generator, 2);
	mpz_set_str(priv->prime, group_info->modp_str, 16);
	printf("Selected group : %d\nPrime : ", group);
	mpz_out_str(stdout, 16, priv->prime);
	printf("\n\n");
	priv->group = group;
}

void dh_rand_init(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;

	uint32_t rnd;
	int fd;
	fd = open("/dev/random", O_RDONLY);
	read(fd, &rnd, sizeof(rnd));
	close(fd);

	gmp_randinit_default(priv->rng_stat);
	gmp_randseed_ui(priv->rng_stat, rnd);
}

void dh_rand(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	dh_param_t * group_info = &priv->group_info[priv->group];

	mpz_urandomb(priv->exp, priv->rng_stat, group_info->bits);
}

void dh_g_x_mod(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;

	mpz_powm(priv->gx_modp, priv->generator, priv->exp, priv->prime);
}

mpz_t * dh_g_xy_mod(dh_t * this, void * chunk)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	dh_param_t * group_info = &priv->group_info[priv->group];

	mpz_t gy;
	mpz_init(gy);

	mpz_import(gy, group_info->bytes, 1, 1, 0, 0, chunk);

	mpz_powm(priv->gxy_modp, gy, priv->exp, priv->prime);

	return &priv->gxy_modp;
}

mpz_t * dh_get_g_x_mod(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;

	return &priv->gx_modp;
}

void * dh_export_gx(dh_t * this, size_t * size)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	int bits;
	int bytes;

	bits = mpz_sizeinbase(priv->gxy_modp, 2);
	bytes = bits/8;

	if(bits)
		return mpz_export(NULL, size, 1, 1, 1, 0, priv->gx_modp);

	return NULL;
}

void * dh_export_gxy(dh_t * this, size_t * size)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	int bits;
	int bytes;

	bits = mpz_sizeinbase(priv->gxy_modp, 2);
	bytes = bits/8;

	if(bits)
		return mpz_export(NULL, size, 1, 1, 1, 0, priv->gxy_modp);

	return NULL;
}

void dh_dump(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	printf("EXP : ");
	mpz_out_str(stdout, 16, priv->exp);
	printf("\n");
	printf("g^x mod p : ");
	mpz_out_str(stdout, 16, priv->gx_modp);
	printf("\n");
	printf("g^xy mod p : ");
	mpz_out_str(stdout, 16, priv->gxy_modp);
	printf("\n");
}

void dh_destroy(dh_t * this)
{
	free(this);
}

dh_t * create_dh(int group)
{
	priv_dh_t * private;
	dh_t * public;

	private = galloc(sizeof(priv_dh_t));
	public = &private->public;

	private->group_info = dh_param;

	public->set_group = dh_set_group;
	public->rand_init = dh_rand_init;
	public->rand = dh_rand;
	public->export_gx = dh_export_gx;
	public->export_gxy = dh_export_gxy;

	public->g_x_mod = dh_g_x_mod;
	public->g_xy_mod = dh_g_xy_mod;

	public->get_g_x_mod = dh_get_g_x_mod;

	mpz_init(private->prime);
	mpz_init(private->generator);
	mpz_init(private->exp);
	mpz_init(private->gx_modp);
	mpz_init(private->gxy_modp);

	public->rand_init(public);
	public->set_group(public, group);
	public->dump = dh_dump;
	public->destroy = dh_destroy;

	return public;
}
