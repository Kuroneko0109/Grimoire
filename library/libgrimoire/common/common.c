#include <libgrimoire/common/common.h>
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
		if((i % 48 == 0) && (i != size))
			printf("\n");
	}
	printf("\n");

	for(i=0;i<pc-1;i++)
		printf("=");
	printf("\n\n");
}
