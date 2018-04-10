#include "map_file.h"

void map_file(FILE *file,listNode **info,char *filename)
{
	listNode *cur = *info;
	int x,i=0;
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(filename, cur->name))
		{
			printf("FOUND node %s\n",cur->name);
			break;
		}
	}
	char *buff = malloc(sizeof(char)*(cur->max_chars+1));
	printf("MAX IS %d\n", cur->max_chars);
	// int *offset_array= malloc(sizeof(int)*cur->lines);
	// int total_offset = 0;
	// offset_array[0] = 0;
	for (i=0;i<cur->lines;i++)
	{
		fgets(buff, cur->max_chars+1, file);
		cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
		strncpy(cur->map[i], buff, strlen(buff));
		// if (i!=0)
		// 	offset_array[i] = total_offset;
		// total_offset += strlen(buff);
		// printf("%s", cur->map[i]);

	}
	free(buff);
	// printf("\n");
}