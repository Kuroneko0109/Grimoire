#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/nlp/knowledge.h>
#include <stdlib.h>

typedef struct _knowledge priv_knowledge_t;
struct _knowledge {
	knowledge_t public;
	
	list_t * dictionary;
};

int knowledge_input(knowledge_t * this, char * word)
{
	priv_knowledge_t * priv = (priv_knowledge_t *)this;
	list_t * dictionary = priv->dictionary;

	if(dictionary->find(dictionary, word))
		return -1;

	dictionary->enqueue_data(dictionary, word);
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
	list_t * dictionary = priv->dictionary;

	dictionary->dump(dictionary);
	printf("Dictionary size : %d\n", dictionary->count(dictionary));
}

knowledge_t * create_knowledge(void)
{
	priv_knowledge_t * private;
	knowledge_t * public;

	private = malloc(sizeof(priv_knowledge_t));
	public = &private->public;

	private->dictionary = create_list(NULL,
			knowledge_list_compare_method,
			knowledge_list_dump_method);

	public->input = knowledge_input;
	public->dump = knowledge_dump;

	return public;
}
