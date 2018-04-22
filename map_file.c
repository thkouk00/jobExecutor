#include "map_file.h"

void map_file(FILE *file,listNode **info,char *filename)
{
	// printf("IN MAP\n");
	listNode *cur = *info;
	int x,i=0;
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(cur->name,filename))
		{
			break;
		}
	}
	char *buff = malloc(sizeof(char)*(cur->max_chars+1));
	for (i=0;i<cur->lines;i++)
	{
		fgets(buff, cur->max_chars+1, file);
		if (buff[strlen(buff)-1] =='\n')
		{
			cur->map[i] = malloc(sizeof(char)*(strlen(buff)));
			strncpy(cur->map[i], buff, strlen(buff)-1);

			cur->map[i][strlen(buff)-1] = '\0';
		}	
		else
		{
			cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
			strncpy(cur->map[i], buff, strlen(buff));
		}
		// printf("MAP:%s\n", buff);

	}
	free(buff);
}