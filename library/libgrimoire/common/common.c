#include <libgrimoire/common/common.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void trimnl(char * str)
{
	int i;

	for(i=0;str[i];i++)
	{
		if(str[i]=='\n')
		{
			str[i]='\0';
			break;
		}
	}
}

void binary_dump(char * title, uint8_t * buffer, int size)
{
	int i;
	int pc;

	pc = printf("========== %s ==========\n", title);

	for(i=1;i<=size;i++)
	{
		printf("%02x", buffer[i-1]);
		if(i % 8 == 0)
			printf(" ");
		if((i % (8*6) == 0) && (i != size))
			printf("\n");
	}
	printf("\n");

	for(i=0;i<pc-1;i++)
		printf("=");
	printf("\n\n");
}

typedef struct priv_logger priv_logger_t;
struct priv_logger {
	logger_t public;

	int level;

	int (*method)(const char *, ...);
};

char * msg_head[] = {
	[LOG_ERROR] = "[ERROR]",
	[LOG_CRITICAL] = "[CRITICAL]",
	[LOG_WARNING] = "[WARNING]",
	[LOG_NOTICE] = "[NOTICE]",
	[LOG_INFO] = "[INFO]",
	[LOG_DEBUG] = "[DEBUG]",
};
int logger_log(logger_t * this, int level, const char * fmt, ...)
{
	priv_logger_t * priv = (priv_logger_t *)this;
	char * buffer;

	if(level < priv->level)
		return -1;

	buffer = malloc(strlen(fmt) + strlen(msg_head[level]));
	strcpy(buffer, fmt);
	strcat(buffer, msg_head[level]);

	return priv->method(fmt, __VA_ARGS__);
}

void logger_set_level(logger_t * this, int level)
{
	priv_logger_t * priv = (priv_logger_t *)this;

	priv->level = level;
}

logger_t * create_logger(void)
{
	logger_t * public;
	priv_logger_t * private;

	private = malloc(sizeof(priv_logger_t));
	public = &private->public;

	private->level = 0;

	public->log = logger_log;
	public->set_level = logger_set_level;

	return public;
}
