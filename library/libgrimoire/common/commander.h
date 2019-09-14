#ifndef _COMMANDER_
#define _COMMANDER_

typedef struct commander commander_t;
struct commander {
	int (*exec_command)(commander_t *, void *, const char *);
	int (*register_command)(commander_t *, const char *, int (*)(void *, const char *), const char *);
	void (*dump)(commander_t *);
};

commander_t * create_command(void);

#endif
