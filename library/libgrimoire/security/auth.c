#include <libgrimoire/security/auth.h>
#include <libgrimoire/common/common.h>
#include <stdio.h>

void p1_init_dump(p1_init_t * msg)
{
	binary_dump("p1init.id", msg->id, sizeof(msg->id));
	binary_dump("p1init.hmac_id", msg->hmac_id, sizeof(msg->hmac_id));
	binary_dump("p1init.k", msg->k, sizeof(msg->k));
}

void p1_resp_dump(p1_resp_t * msg)
{
	binary_dump("p1resp.hmac_k", msg->hmac_k, sizeof(msg->hmac_k));
	binary_dump("p1resp.modp", msg->modp, sizeof(msg->modp));
	binary_dump("p1resp.hmac_modp", msg->hmac_modp, sizeof(msg->hmac_modp));
}

void p2_init_dump(p2_init_t * msg)
{
	binary_dump("p2init.modp", msg->modp, sizeof(msg->modp));
	binary_dump("p2init.hmac_modp", msg->hmac_modp, sizeof(msg->hmac_modp));
}
