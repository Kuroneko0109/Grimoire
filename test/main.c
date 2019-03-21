#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgrimoire/system/thread.h>
#include <libgrimoire/grid/peer.h>
#include <libgrimoire/grid/acceptor.h>
#include <libgrimoire/math/vector.h>
#include <libgrimoire/datastructure/iterator.h>
#include <libgrimoire/nlp/knowledge.h>
#include <libgrimoire/config/config.h>
#include <libgrimoire/system/cpu.h>

#include <jansson.h>
#include <iconv.h>

void * collector(void * data);

void * garbage(void * data);

int main(int argc, char * argv[])
{
	cpu_t * cpu;
	config_t * config_test = create_config(argv[1], 8192);
	if(config_test)
		init_cpu(config_test);

	cpu = get_global_cpu();
	cpu->dump_info();

	cpu->task_register(
			create_task("", collector, argv[2], NULL),
			1);

	cpu->drive();

	return 0;
}

void * collector(void * param)
{
	knowledge_t * knowledge;

	FILE * f;
	json_t * root;
	json_t * data;

	json_t * dataarray;
	json_t * obj;

	json_t * paragraphs;
	json_t * qa;
	json_t * context;

	json_error_t err;

	char buffer[8192];

	knowledge = create_knowledge();

	f = fopen(param, "r");
	printf("f : %p\n", f);
	if(!f)
		return 0;
	root = json_loadf(f, 0, &err);
	printf("root : %p\n", root);
	if(!root)
		return 0;

	/* this is array */
	data = json_object_get(root, "data");
	printf("data : %p\n", data);
	if(!data)
		return 0;

	int paragraph_count = 0;
	int i;
	knowledge_t * crashed_knowledge = create_knowledge();
	for(i=0;i<json_array_size(data);i++)
	{
		obj = json_array_get(data, i);
		if(!obj)
			return 0;
	
		/* this is array */
		paragraphs = json_object_get(obj, "paragraphs");
		if(!paragraphs)
			return 0;
	
		int j;
		printf("json array size : %d\n", json_array_size(paragraphs));
		for(j=0;j<json_array_size(paragraphs);j++)
		{
			qa = json_array_get(paragraphs, j);
			if(!qa)
				return 0;
	
			context = json_object_get(qa, "context");
	
			strcpy(buffer, json_string_value(context));
	
			char * tok;
			char * tmp;
			int len;
			tok = strtok(buffer, " ");
			while(tok)
			{
				len = strlen(tok)+1;
				tmp = malloc(len);
				memset(tmp, 0, len);
				strcpy(tmp, tok);
				if(knowledge->input(knowledge, tmp))
				{
//					free(tmp);
					if(crashed_knowledge->input(crashed_knowledge, tmp))
						free(tmp);
				}
				tok = strtok(NULL, " ");
			}
		}
		printf("%d'th paragraph\n", paragraph_count);
		paragraph_count++;
	}

	knowledge->dump(knowledge);
	crashed_knowledge->dump(crashed_knowledge);
	printf("\n\nend...\n");

	fclose(f);

	return NULL;
}
