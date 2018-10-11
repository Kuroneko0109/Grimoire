#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgrimoire/security/dh.h>

typedef struct dh_param dh_param_t;

struct dh_param {
	char * modp_str;
	int bits;
	int bytes;
	int supported;
};

const dh_param_t dh_param[32] = {
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
	mpz_t rn;
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
	mpz_clear(priv->generator);
	mpz_init(priv->generator);
	mpz_init(priv->prime);
	mpz_init(priv->generator);

	mpz_set_ui(priv->generator, 2);
	mpz_set_str(priv->prime, group_info->modp_str, 16);
	printf("Selected group : %d\nPrime : ", group);
	mpz_out_str(stdout, 16, priv->prime);
	printf("\n");
	priv->group = group;
}

void dh_rand_init(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;

	srand(time(NULL));
	gmp_randinit_default(priv->rng_stat);
	gmp_randseed_ui(priv->rng_stat, rand());
}

void dh_rand(dh_t * this)
{
	priv_dh_t * priv = (priv_dh_t *)this;
	dh_param_t * group_info = &priv->group_info[priv->group];

	mpz_urandomb(priv->rn, priv->rng_stat, group_info->bits);
	printf("RN : ");
	mpz_out_str(stdout, 16, priv->rn);
	printf("\n");
}

dh_t * create_dh(int group)
{
	priv_dh_t * private;
	dh_t * public;

	private = malloc(sizeof(priv_dh_t));
	public = &private->public;

	private->group_info = dh_param;

	public->set_group = dh_set_group;
	public->rand_init = dh_rand_init;
	public->rand = dh_rand;

	mpz_init(private->prime);
	mpz_init(private->generator);
	mpz_init(private->exp);
	mpz_init(private->rn);

	public->rand_init(public);
	public->set_group(public, group);

	return public;
}
