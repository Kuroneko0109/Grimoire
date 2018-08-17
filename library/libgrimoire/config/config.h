#ifndef _CONFIG_
#define _CONFIG_

#include <libgrimoire/common/common.h>

typedef struct config_element config_element_t;

struct config_element {
	char key[32];
	char val[64];
};

typedef struct config config_t;

struct config {
	config_element_t * (*get_value)(config_t * this, char * key);
};

config_element_t * create_config_element(char * key, char * val);
config_t * create_config(char * directory, int buffer_len);

#endif
