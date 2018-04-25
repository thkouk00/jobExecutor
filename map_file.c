#include "map_file.h"

void map_file(FILE *file,listNode **info,char *filename)
{
	listNode *cur = *info;
	int x,i=0;
	printf("**BEF WHILEE**\n");
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(cur->name,filename))
		{
			break;
		}
	}
	char *buff = NULL;
	size_t buff_size;
	int loop=0;
	for (i=0;i<cur->lines;i++)
	{
		getline(&buff,&buff_size,file);
		
		if (buff[strlen(buff)-1] =='\n')
		{
			cur->map[i] = malloc(sizeof(char)*(strlen(buff)));	//htan xwris 1
			memset(cur->map[i], 0, strlen(buff));
			memcpy(cur->map[i], buff, strlen(buff)-1);
		}	
		else
		{
			cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
			memset(cur->map[i], 0, strlen(buff)+1);
			memcpy(cur->map[i], buff, strlen(buff));
		}
		free(buff);
		buff = NULL;
	}
	// free(buff);
}