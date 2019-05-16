#include <libgrimoire/datastructure/hashlist.h>
#include <libgrimoire/nlp/knowledge.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct _knowledge priv_knowledge_t;
struct _knowledge {
	knowledge_t public;
	
	hashlist_t * dictionary;
};

int knowledge_input(knowledge_t * this, char * word)
{
	priv_knowledge_t * priv = (priv_knowledge_t *)this;
	hashlist_t * dictionary = priv->dictionary;

	if(dictionary->find_data(dictionary, word))
		return -1;

	dictionary->input_data(dictionary, word);
	return 0;
}

int knowledge_list_compare_method(void * d, void * s)
{
	char * sword = s;
	char * dword = d;

	return strcmp(sword, dword);
}

void * knowledge_list_dump_method(void * data)
{
	char * word = data;
	printf("\"%s\", ", word);

	return NULL;
}

void knowledge_dump(knowledge_t * this)
{
	priv_knowledge_t * priv = (priv_knowledge_t *)this;
	hashlist_t * dictionary = priv->dictionary;
	list_t * merged;
	
//	dictionary->dump(dictionary);
	dictionary->count(dictionary);

	printf("start mergeing...\n");
	merged = dictionary->merge(dictionary);
	printf("ok...\n");
	merged->count(merged);
}

unsigned int knowledge_string_hasher(void * data)
{
	char * string = data;
	int i;
	int j;
	unsigned int ret = 0;
	char * p = (char *)&ret;
	int len = strlen(string);

	j = 0;
	for(i=0;i<len;i++)
	{
		p[j] ^= string[i];
		j = (j+1)%sizeof(ret);
	}

	return ret;
}

void * knowledge_copy(void * data)
{
	char * ret;
	ret = malloc(strlen(data) + 1);
	strcpy(ret, data);
	return (void *)ret;
}

knowledge_t * create_knowledge(unsigned int hash_size)
{
	priv_knowledge_t * private;
	knowledge_t * public;

	private = malloc(sizeof(priv_knowledge_t));
	public = &private->public;
	private->dictionary = create_hashlist(
			knowledge_string_hasher,
			hash_size,
			NULL,
			knowledge_list_compare_method,
			knowledge_list_dump_method);

	private->dictionary->set_copy(private->dictionary, knowledge_copy);

	public->input = knowledge_input;
	public->dump = knowledge_dump;

	return public;
}
