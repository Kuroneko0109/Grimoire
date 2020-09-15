#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgrimoire/delivery/delivery.h>
#include <libgrimoire/delivery/mailbox.h>
#include <libgrimoire/system/memory.h>

typedef struct priv_delivery priv_delivery_t;

struct priv_delivery {
	delivery_t public;

	list_t * boxlist;
};

priv_delivery_t * delivery_global;

int delivery_mailbox_compare(void * src, void * dst)
{
	mailbox_t * srcbox = src;

	char * src_name = srcbox->get_name(srcbox);
	char * dst_name = dst;

	return strcmp(src_name, dst_name);
}

void delivery_send_to(delivery_t * this, char * boxname, mail_t * mail)
{
	priv_delivery_t * priv = (priv_delivery_t *)this;
	list_t * boxlist = priv->boxlist;
	mailbox_t * mailbox;

	boxlist->lock(boxlist);

	mailbox = boxlist->find_data(boxlist, delivery_mailbox_compare, boxname);
	if(mailbox)
		mailbox->input(mailbox, mail);
	else
	{
		printf("Unregistered mailbox name %s. Mail destroyed\n", boxname);
		free(mail);
	}
	boxlist->unlock(boxlist);
}

mailbox_t * delivery_register_mailbox(delivery_t * this, char * boxname)
{
	priv_delivery_t * priv = (priv_delivery_t *)this;
	list_t * boxlist = priv->boxlist;
	mailbox_t * mailbox = create_mailbox(boxname);

	boxlist->lock(boxlist);
	boxlist->enqueue_data(boxlist, mailbox);
	boxlist->unlock(boxlist);

	return mailbox;
}

void delivery_destroy(delivery_t * this)
{
	free(this);
}

void __attribute__((constructor)) init_delivery(void)
{
	printf("Grimoire delivery initializing...\n");
	delivery_global = galloc(sizeof(priv_delivery_t));

	delivery_global->public.send_to = delivery_send_to;
	delivery_global->public.register_mailbox = delivery_register_mailbox;
	delivery_global->public.destroy = delivery_destroy;

	delivery_global->boxlist = create_list(LOCK_MUTEX, NULL, NULL);
}

delivery_t * get_delivery_global(void)
{
	return &delivery_global->public;
}
