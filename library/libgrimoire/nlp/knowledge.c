#include <libgrimoire/datastructure/hashlist.h>
#include <libgrimoire/nlp/knowledge.h>
#include <stdlib.h>
#include <stdint.h>

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
	
	dictionary->dump(dictionary);
}

unsigned int knowledge_string_hasher(char * string)
{
	int i;
	char ret = 0;
	int len = strlen(string);

	for(i=0;i<len;i++)
		ret ^= string[i];

	return (unsigned int)ret;
}

knowledge_t * create_knowledge(void)
{
	priv_knowledge_t * private;
	knowledge_t * public;

	private = malloc(sizeof(priv_knowledge_t));
	public = &private->public;
	private->dictionary = create_hashlist(
			knowledge_string_hasher,
			256,
			NULL,
			knowledge_list_compare_method,
			knowledge_list_dump_method);

	public->input = knowledge_input;
	public->dump = knowledge_dump;

	return public;
}
