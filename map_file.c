#include "map_file.h"

void map_file(FILE *file,listNode **info,char *filename,int max)
{
	listNode *cur = *info;
	int x,i=0;
	char *buff = malloc(sizeof(char)*(max+1));
	printf("MAX IS %d\n", max);
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(filename, cur->name))
		{
			printf("FOUND node %s\n",cur->name);
			break;
		}
	}
	for (i=0;i<cur->lines;i++)
	{
		fgets(buff, max+1, file);
		cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
		strncpy(cur->map[i], buff, strlen(buff));
		// printf("%s", cur->map[i]);

	}
	free(buff);
	// printf("\n");
}