#include "map_file.h"

void map_file(FILE *file,listNode **info,char *filename)
{
	listNode *cur = *info;
	int x,i=0;
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(cur->name,filename))
		{
			// printf("FOUND node %s\n",cur->name);
			// printf("INMAPFILE %s\n", cur->name);
			break;
		}
	}
	char *buff = malloc(sizeof(char)*(cur->max_chars+1));
	// printf("MAX IS %d and lines %d\n", cur->max_chars,cur->lines);
	for (i=0;i<cur->lines;i++)
	{
		fgets(buff, cur->max_chars+1, file);
		// cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
		// printf("size %ld\n", strlen(buff));
		// if (buff[strlen(buff)-1] =='\n')
		// 	printf("***%s\n", buff);
		if (buff[strlen(buff)-1] =='\n')
		{
			cur->map[i] = malloc(sizeof(char)*(strlen(buff)));
			strncpy(cur->map[i], buff, strlen(buff)-1);

			cur->map[i][strlen(buff)-1] = '\0';
			// cur->map[i][strlen(buff)] = '\0';
		}	
		else
		{
			cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
			strncpy(cur->map[i], buff, strlen(buff));
			// cur->map[i][strlen(buff)+1] = '\0';
		}
		// printf("%s\n", cur->map[i]);
		

	}
	free(buff);
	// printf("\n");
}