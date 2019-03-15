#ifndef _KNOWLEDGE_
#define _KNOWLEDGE_

typedef struct knowledge knowledge_t;
struct knowledge {
	int (*input)(knowledge_t * this, char * word);
	void (*dump)(knowledge_t * this);
};

knowledge_t * create_knowledge(void);

#endif
