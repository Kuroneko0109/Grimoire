#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libgrimoire/config/config.h>
#include <libgrimoire/common/common.h>
#include <libgrimoire/system/file.h>
#include <libgrimoire/datastructure/list.h>

config_element_t * create_config_element(char * key, char * val)
{
	config_element_t * element = malloc(sizeof(config_element_t));
	strcpy(element->key, key);
	strcpy(element->val, val);
	return element;
}

typedef struct priv_config priv_config_t;

struct priv_config {
	config_t public;

	void (*parser)(config_t * this);

	file_t * file;
	list_t * list;

	int buffer_len;
	char buffer[0];
};

void config_parser(config_t * config)
{
	priv_config_t * priv = (priv_config_t *)config;
	char str[256];

	char key[64];
	char val[192];

	char * ptr = priv->buffer;
	int i;

	ptr = strtok(priv->buffer, "\n");
	while(ptr)
	{
		if(ptr[0] != '#')
		{
			strcpy(str, ptr);
			memset(key, 0, sizeof(key));
			memset(val, 0, sizeof(val));

			for(i=0;str[i]!='=';i++)
				key[i] = str[i];	// strcpy for front '='
			i++;	// next '='
			strcpy(val, str+i);

			if('\0' == key[0] || '\0' == val[0])
				printf("Invalid config line(%s)\n", str);
			else
				priv->list->enqueue_data(priv->list, create_config_element(key, val));
		}
		ptr = strtok(NULL, "\n");
	}
}

int config_compare_by_key(void * _s, void * _d)
{
	config_element_t * s = _s;
	config_element_t * d = _d;
	return strcmp(s->key, d->key);
}

char * config_get_value(config_t * this, char * key)
{
	priv_config_t * priv = (priv_config_t *)this;
	config_element_t obj;
	node_t * node;

	strcpy(obj.key, key);

	node = priv->list->find(priv->list, config_compare_by_key, &obj);
	if(node)
		return ((config_element_t *)node->get_data(node))->val;

	return NULL;
}

void * config_dump_element(void * data)
{
	config_element_t * element = (config_element_t *)data;

	printf("key : %s, val : %s\n", element->key, element->val);

	return NULL;
}

void config_reload(config_t * this)
{
	priv_config_t * priv = (priv_config_t *)this;
	file_t * file;
	list_t * list;

	memset(priv->buffer, 0, priv->buffer_len);

	file = priv->file;
	file->open(file);
	file->read(file, priv->buffer, priv->buffer_len);
	printf("priv->buffer : %s", priv->buffer);
	file->close(file);

	list = priv->list;
	list->flush(list);

	priv->parser(this);
}

void config_dump(config_t * this)
{
	priv_config_t * priv = (priv_config_t *)this;

	priv->list->lock(priv->list);
	priv->list->dump(priv->list);
	priv->list->unlock(priv->list);
}

config_t * create_config(char * directory, int buffer_len)
{
	printf("Load Config(%s)...\n", directory);

	list_t * list;

	priv_config_t * private = malloc(sizeof(priv_config_t) + buffer_len);
	config_t * public = &private->public;

	private->buffer_len = buffer_len;
	private->parser = config_parser;
	private->file = create_file(directory);
	private->list = create_list(NULL, config_dump_element);
	memset(private->buffer, 0, private->buffer_len);

	public->get_value = config_get_value;
	public->reload = config_reload;
	public->dump = config_dump;

	public->reload(public);

	private->parser(public);

	public->dump(public);

	return public;
}
