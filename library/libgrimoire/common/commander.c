#include <libgrimoire/common/commander.h>
#include <libgrimoire/common/command_word.h>
#include <libgrimoire/system/memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct priv_commander priv_commander_t;
struct priv_commander {
	commander_t public;

	command_word_t * root_word;
};

int commander_register_command(commander_t * this, const char * command, int (*func)(void *, const char *), const char * description)
{
	priv_commander_t * priv = (priv_commander_t *)this;

	char string_buffer[1024] = {0, };
	char * tok;
	command_word_t * word;
	command_word_t * next_word;

	strncpy(string_buffer, command, sizeof(string_buffer));

	tok = strtok(string_buffer, " ");
	word = priv->root_word;
	next_word = word;
	while(tok)
	{
		next_word = word->find_next_word(word, tok);
		if(NULL == next_word)
			next_word = word->register_next(word, tok, NULL, NULL);

		tok = strtok(NULL, " ");
		if(NULL == tok)
		{
			next_word->set_func(next_word, func);
			next_word->set_description(next_word, description);
			break;
		}
		else if(!strcmp("[param]", tok))
		{
			next_word->set_func(next_word, func);
			next_word->set_description(next_word, description);
			next_word->use_next_param(next_word, 1);
			break;
		}

		word = next_word;
	}

	return 0;
}

int commander_exec_command(commander_t * this, void * result, const char * command)
{
	priv_commander_t * priv = (priv_commander_t *)this;

	char string_buffer[1024] = {0, };
	char * tok;
	char * param_tok;
	command_word_t * word;
	command_word_t * next_word;

	strncpy(string_buffer, command, sizeof(string_buffer));

	tok = strtok(string_buffer, " ");
	word = priv->root_word;
	next_word = word;
	while(tok)
	{
		next_word = word->find_next_word(word, tok);
		if(NULL == next_word)
		{
			printf("Invalid command!\n");
			return -1;
		}

		if(next_word->need_next_param(next_word))
		{
			param_tok = strtok(NULL, "\0");

			return next_word->exec(next_word, result, param_tok);
		}

		tok = strtok(NULL, " ");
		word = next_word;
		if(NULL == tok)
			word->exec(word, result, "");
	}

	return 0;
}

void commander_get_dumpstr(commander_t * this)
{
}

void commander_dump(commander_t * this)
{
	priv_commander_t * priv = (priv_commander_t *)this;

	priv->root_word->dump(priv->root_word, "");
}

commander_t * create_commander(void)
{
	priv_commander_t * private;
	commander_t * public;

	private = galloc(sizeof(priv_commander_t));
	public = &private->public;

	private->root_word = create_command_word("/", NULL, NULL);

	public->exec_command = commander_exec_command;
	public->register_command = commander_register_command;

	public->dump = commander_dump;

	return public;
}
