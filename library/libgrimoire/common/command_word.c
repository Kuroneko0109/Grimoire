#include <libgrimoire/datastructure/list.h>
#include <libgrimoire/common/command_word.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct priv_command_word priv_command_word_t;
struct priv_command_word {
	command_word_t public;

#define COMMAND_WORD_STR_SIZE	(64)
#define COMMAND_HELP_STR_SIZE	(512)
	char word[COMMAND_WORD_STR_SIZE];
	char description[COMMAND_HELP_STR_SIZE];
	int (*func)(void *, const char * str);

	int use_next_param;

	list_t * next_word_list;
};

int command_word_compare(void * s, void * d)
{
	priv_command_word_t * priv = (priv_command_word_t *)s;
	char * wordstr = (char *)d;
	
	return strncmp(priv->word, wordstr, sizeof(priv->word));
}

int command_word_exec(command_word_t * this, void * result, const char * strparam)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;

	return priv->func(result, strparam);
}

void command_word_set_func(command_word_t * this, int (*func)(void *, const char *))
{
	priv_command_word_t * priv = (priv_command_word_t *)this;

	priv->func = func;
}

int command_word_default_func(void * result, const char * strparam)
{
	printf("Incomplete command!\n");
	
	return -1;
}

void command_word_set_description(command_word_t * this, const char * description)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;

	if(description)
		strncpy(priv->description, description, sizeof(priv->description));
}

command_word_t * command_word_register_next(command_word_t * this,
		const char * wordstr, int (*func)(void *, const char *), const char * description)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;
	command_word_t * word = create_command_word(wordstr, func, description);

	priv->next_word_list->enqueue_data(priv->next_word_list, word);

	return word;
}

command_word_t * command_word_find_next_word(command_word_t * this, const char * wordstr)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;

	return priv->next_word_list->find_data(priv->next_word_list, command_word_compare, (void *)wordstr);
}

void command_word_dump(command_word_t * this, const char * prefix)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;
	iterator_t * iterator;
	command_word_t * child;
	char * next_prefix[256] = {0, };

	if(strlen(priv->description))
		printf("%s|- %s\t: %s\n", prefix, priv->word, priv->description);
	else
		printf("%s|- %s\t\n", prefix, priv->word);

	if(prefix)
		sprintf(next_prefix, "%s\t", prefix);

	iterator = priv->next_word_list->get_iterator(priv->next_word_list);
	while((child = iterator->next(iterator)))
		child->dump(child, (const char *)next_prefix);

	iterator->destroy(iterator);
}

void command_word_use_next_param(command_word_t * this, int use)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;
	priv->use_next_param = use;
}

int command_word_need_next_param(command_word_t * this)
{
	priv_command_word_t * priv = (priv_command_word_t *)this;
	return priv->use_next_param;
}

command_word_t * create_command_word(const char * wordstr, int (*func)(void *, const char *), const char * description)
{
	priv_command_word_t * private;
	command_word_t * public;

	private = malloc(sizeof(priv_command_word_t));
	public = &private->public;

	memset(private->word, 0, sizeof(private->word));
	strncpy(private->word, wordstr, sizeof(private->word));
	private->func = command_word_default_func;
	if(func)
		private->func = func;

	if(description)
		strncpy(private->description, description, sizeof(private->description));

	private->next_word_list = create_list(NULL, NULL);

	public->exec = command_word_exec;
	public->set_func = command_word_set_func;
	public->set_description = command_word_set_description;

	public->register_next = command_word_register_next;
	public->find_next_word = command_word_find_next_word;

	public->use_next_param = command_word_use_next_param;
	public->need_next_param = command_word_need_next_param;

	public->dump = command_word_dump;

	return public;
}
