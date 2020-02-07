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
	ssize_t (*size)(file_t * this);
	void (*fsync)(file_t * this);
	void (*set_path)(file_t * this, const char * path);
	void (*close)(file_t * this);
	void (*remove)(file_t * this);
	void (*destroy)(file_t * this);
};

file_t * create_file(const char * path);

#endif
