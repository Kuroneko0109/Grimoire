#ifndef _DB_
#define _DB_

typedef struct db db_t;
struct db {
	int (*squery)(db_t *, const char *);	// Sync query
	int (*aquery)(db_t *, const char *);	// Async query
	void (*set_conn_info)(db_t *, const char *, unsigned int, const char *, const char *, const char *);
	int (*get_error)(db_t *, char *);
	int (*connect)(db_t *);
	void (*disconnect)(db_t *);
	void (*destroy)(db_t *);
};

db_t * create_db(void);

#endif
