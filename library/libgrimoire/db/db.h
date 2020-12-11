#ifndef _DB_
#define _DB_

typedef struct db db_t;
struct db {
	int (*query)(db_t *, const char *);
	int (*fquery)(db_t *, const char *);
	void (*set_conn_info)(db_t *, const char *, unsigned int, const char *, const char *, const char *);
	int (*connect)(db_t *);
	void (*disconnect)(db_t *);
	void (*destroy)(db_t *);
};

db_t * create_db(void);

#endif
