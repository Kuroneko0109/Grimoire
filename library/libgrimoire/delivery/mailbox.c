#include <stdlib.h>
#include <libgrimoire/delivery/mailbox.h>
#include <libgrimoire/datastructure/list.h>

typedef struct priv_mailbox priv_mailbox_t;

struct priv_mailbox {
	mailbox_t public;

	char boxname[32];
	list_t * mail_list;
};

char * mailbox_get_name(mailbox_t * this)
{
	priv_mailbox_t * priv = (priv_mailbox_t *)this;

	return priv->boxname;
}

int mailbox_input(mailbox_t * this, mail_t * mail)
{
	priv_mailbox_t * priv = (priv_mailbox_t *)this;
	list_t * mail_list = priv->mail_list;

	mail_list->lock(mail_list);
	mail_list->enqueue_data(mail_list, mail);
	mail_list->unlock(mail_list);

	return 0;
}

void mailbox_destroy(mailbox_t * this)
{
}

mailbox_t * create_mailbox(char * boxname)
{
	priv_mailbox_t * private;
	mailbox_t * public;

	private = malloc(sizeof(priv_mailbox_t));
	public = &private->public;

	public->destroy = mailbox_destroy;
	public->get_name = mailbox_get_name;
	public->input = mailbox_input;

	private->mail_list = create_list(NULL, NULL, NULL);

	return public;
}

mail_t * create_mail(int datasize)
{
	mail_t * mail;

	mail = malloc(sizeof(mail_t) + datasize);
	mail->datasize = datasize;

	return mail;
}
