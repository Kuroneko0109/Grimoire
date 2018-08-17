#include <stdlib.h>
#include <string.h>
#include <libgrimoire/delivery/delivery.h>
#include <libgrimoire/delivery/mailbox.h>

typedef struct priv_delivery priv_delivery_t;

struct priv_delivery {
	delivery_t public;

	list_t * boxlist;
};

priv_delivery_t * delivery_global;

void delivery_send_to(delivery_t * this, char * boxname, mail_t * mail)
{
	priv_delivery_t * priv = (priv_delivery_t *)this;
	list_t * boxlist = priv->boxlist;
	mailbox_t * mailbox;

	boxlist->lock(boxlist);

	mailbox = boxlist->find_data(boxlist, boxname);
	if(mailbox)
		mailbox->input(mailbox, mail);
	else
	{
		printf("Unregistered mailbox name : %s\n", boxname);
		free(mail);
	}
	boxlist->unlock(boxlist);
}

mailbox_t * delivery_takeout_mailbox(delivery_t * this, char * boxname)
{
	priv_delivery_t * priv = (priv_delivery_t *)this;
	mailbox_t * mailbox;

	return mailbox;
}

void delivery_put_mailbox(delivery_t * this, char * boxname)
{
	priv_delivery_t * priv = (priv_delivery_t *)this;
	list_t * boxlist = priv->boxlist;

	create_mailbox(boxname);
}

void delivery_destroy(delivery_t * this)
{
	free(this);
}

int delivery_mailbox_compare(void * src, void * dst)
{
	mailbox_t * srcbox = src;
	mailbox_t * dstbox = dst;

	char * src_name = srcbox->get_name(srcbox);
	char * dst_name = dstbox->get_name(dstbox);

	return strcmp(src_name, dst_name);
}

void __attribute__((constructor)) init_delivery(void)
{
	printf("Grimoire delivery initializing...\n");
	delivery_global = malloc(sizeof(priv_delivery_t));

	delivery_global->public.send_to = delivery_send_to;
	delivery_global->public.takeout_mailbox = delivery_takeout_mailbox;
	delivery_global->public.put_mailbox = delivery_put_mailbox;
	delivery_global->public.destroy = delivery_destroy;

	delivery_global->boxlist = create_list(NULL, delivery_mailbox_compare, NULL);
}

delivery_t * get_delivery_global(void)
{
	return &delivery_global->public;
}
