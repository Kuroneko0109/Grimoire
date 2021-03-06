#include <libgrimoire/system/thread.h>
#include <libgrimoire/system/file.h>
#include <libgrimoire/common/common.h>
#include <libgrimoire/system/memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

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

	int log_mode;	// sync, async

	int (*internal_method)(logger_t *, const char *);

	int level;

	void * exparam;	// commonly, file.

	thread_t * async_thread;
	list_t * async_queue;
};

char * msg_head[] = {
	[LOG_ERROR] = "[ERROR] ",
	[LOG_CRITICAL] = "[CRITICAL] ",
	[LOG_WARNING] = "[WARNING] ",
	[LOG_NOTICE] = "[NOTICE] ",
	[LOG_INFO] = "[INFO] ",
	[LOG_DEBUG] = "[DEBUG] ",
};

int logger_method_printf(logger_t * this, const char * data)
{
	return printf(data);
}

int logger_method_file(logger_t * this, const char * data)
{
	priv_logger_t * priv = (priv_logger_t *)this;
	file_t * file = priv->exparam;

	if(NULL == file)
		return -1;

	file->open(file);	// if file opened, isn't makes problem
	file->seek(file, 0, SEEK_END);
	return file->write(file, (void *)data, strlen(data));
}

int logger_log(logger_t * this, int level, const char * fmt, ...)
{
	priv_logger_t * priv = (priv_logger_t *)this;

	va_list list;
	char * buffer1;
	char * buffer2;
	int ret;

	char * tmp;

	time_t t;
	struct tm lt;

	if(level > priv->level)
		return -1;

	buffer1 = galloc(8192);
	memset(buffer1, 0, 8192);

	buffer2 = buffer1 + 4096;

	t = time(NULL);
	localtime_r(&t, &lt);

	sprintf(buffer1, "%04d-%02d-%02d %02d:%02d:%02d] ",
			lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
			lt.tm_hour, lt.tm_min, lt.tm_sec);
	strcat(buffer1, msg_head[level]);
	strcat(buffer1, fmt);

	va_start(list, fmt);
	vsprintf(buffer2, buffer1, list);
	va_end(list);
	strcat(buffer2, "\n");

	if(LOGMODE_ASYNC == priv->log_mode)
	{
		tmp = galloc(strlen(buffer2)+1);
		memset(tmp, 0, strlen(buffer2)+1);
		strcpy(tmp, buffer2);

		priv->async_queue->lock(priv->async_queue);
		priv->async_queue->enqueue_data(priv->async_queue, tmp);
		priv->async_queue->unlock(priv->async_queue);
	}
	else
		ret = priv->internal_method(this, buffer2);
	free(buffer1);
	return ret;
}

void * logger_async_log(void * param)
{
	logger_t * this = (logger_t *)param;
	priv_logger_t * priv = (priv_logger_t *)this;
	list_t * queue;
	list_t * list;
	void * data;

	queue = priv->async_queue;

	list = create_list(LOCK_MUTEX, NULL, NULL);
	queue->lock(queue);

	while((data = queue->dequeue_data(queue)))
		list->enqueue_data(list, data);

	queue->unlock(queue);

	while((data = list->dequeue_data(list)))
	{
		priv->internal_method(priv, data);
		free(data);
	}

	list->destroy(list);

	return NULL;
}

void logger_assign_logfile(logger_t * this, const char * path)
{
	priv_logger_t * priv = (priv_logger_t *)this;

	priv->exparam = create_file(path);
}

void logger_set_level(logger_t * this, const char * strlevel)
{
	priv_logger_t * priv = (priv_logger_t *)this;
	int level;

	if(0 == strcmp("error", strlevel))
		level = LOG_ERROR;
	else if(0 == strcmp("critical", strlevel))
		level = LOG_CRITICAL;
	else if(0 == strcmp("warning", strlevel))
		level = LOG_WARNING;
	else if(0 == strcmp("notice", strlevel))
		level = LOG_NOTICE;
	else if(0 == strcmp("info", strlevel))
		level = LOG_INFO;
	else if(0 == strcmp("debug", strlevel))
		level = LOG_DEBUG;

	priv->level = level;
}

void logger_set_method(logger_t * this, const char * method_name)
{
	priv_logger_t * priv = (priv_logger_t *)this;

	if(0 == strcmp(method_name, "syslog"))	// tmp printf
		priv->internal_method = logger_method_printf;
	else if(0 == strcmp(method_name, "file"))
		priv->internal_method = logger_method_file;
	else	// default printf
		priv->internal_method = logger_method_printf;
}

void logger_set_mode(logger_t * this, const char * modename)
{
	priv_logger_t * priv = (priv_logger_t *)this;

	int mode = LOGMODE_SYNC;
	if(0 == strcmp(modename, "async"))
		mode = LOGMODE_ASYNC;

	if(LOGMODE_ASYNC == mode)
	{
		priv->log_mode = LOGMODE_ASYNC;
		priv->async_thread = create_thread(NULL);
		priv->async_thread->task_register(priv->async_thread,
				create_task("logging", logger_async_log, this, NULL)
				);
		priv->async_thread->task_register(priv->async_thread,
				create_task("idle", thread_default_idle, this, NULL)
				);
		priv->async_queue = create_list(LOCK_MUTEX, NULL, NULL);

		priv->async_thread->run(priv->async_thread);
	}
	else
		priv->log_mode = LOGMODE_SYNC;
}

logger_t * create_logger(void)
{
	logger_t * public;
	priv_logger_t * private;

	private = galloc(sizeof(priv_logger_t));
	public = &private->public;

	public->log = logger_log;

	public->set_level = logger_set_level;
	public->set_method = logger_set_method;
	public->set_mode = logger_set_mode;
	public->assign_logfile = logger_assign_logfile;

	public->set_mode(public, "sync");
	public->set_level(public, "debug");
	public->set_method(public, "printf");

	return public;
}
