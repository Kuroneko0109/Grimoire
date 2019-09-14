#ifndef _COMMAND_WORD_
#define _COMMAND_WORD_

typedef struct command_word command_word_t;
struct command_word {
	int (*exec)(command_word_t *, void *, const char *);
	void (*set_func)(command_word_t *, int (*func)(void *, const char *));
	void (*set_description)(command_word_t *, const char *);

	command_word_t * (*register_next)(command_word_t *,
			const char *, int (*)(void *, const char *), const char *);
	command_word_t * (*find_next_word)(command_word_t *, const char *);

	void (*use_next_param)(command_word_t *, int);
	int (*need_next_param)(command_word_t *);

	void (*dump)(command_word_t *, const char *);
};

command_word_t * create_command_word(const char *, int (*)(void *, const char *), const char *);

#endif
