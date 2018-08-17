#ifndef _MAILBOX_
#define _MAILBOX_

#include <stdint.h>
#include <libgrimoire/datastructure/list.h>

typedef struct mail mail_t;

struct mail {
	uint32_t datasize;
	uint8_t data[0];
};

typedef struct mailbox mailbox_t;

struct mailbox {
	char * (*get_name)(mailbox_t * this);
	int (*input)(mailbox_t * this, mail_t * mail);
	void (*destroy)(mailbox_t * this);
};

mail_t * create_mail(int datasize);
mailbox_t * create_mailbox(char * boxname);

#endif
