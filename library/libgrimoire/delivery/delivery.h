#ifndef _DELIVERY_
#define _DELIVERY_

#include <libgrimoire/delivery/mailbox.h>

typedef struct delivery delivery_t;

struct delivery {
	void (*send_to)(delivery_t * this, char * boxname, mail_t * mail);
	mailbox_t * (*register_mailbox)(delivery_t * this, char * boxname);
	void (*destroy)(delivery_t * this);
};

delivery_t * get_delivery_global(void);
delivery_t * create_delivery(void);

#endif
