#ifndef _FILE_
#define _FILE_

#include <sys/types.h>

typedef struct file file_t;

struct file {
	int (*exist)(file_t * this);
	void (*creat)(file_t * this);
	int (*open)(file_t * this);
	ssize_t (*read)(file_t * this, void * dst, size_t size);
	ssize_t (*write)(file_t * this, void * src, size_t size);
	void (*fsync)(file_t * this);
	void (*close)(file_t * this);
	void (*remove)(file_t * this);
	void (*destroy)(file_t * this);
};

file_t * create_file(const char * filename);

#endif
