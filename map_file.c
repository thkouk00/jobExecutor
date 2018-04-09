#include "map_file.h"

void map_file(FILE **file,listNode **info,char *filename,int max,int lines)
{
	listNode *cur = *info;
	int x,i=0;
	char *buff = malloc(sizeof(char)*max);
	size_t buff_size;
	while (cur->next)
	{
		cur = cur->next;
		if (!strcmp(filename, cur->name))
			break;
	}
	for (i=0;i<lines;i++)
	{
		fgets(buff, max, *file);
		cur->map[i] = malloc(sizeof(char)*(strlen(buff)+1));
		strncpy(cur->map[i],buff,strlen(buff)+1);
		// printf("%s",cur->map[i]);
	}
}