#include "wc.h"

void wc(listNode **head , int * t_chars,int *t_words , int *t_lines)
{
	int i;
	char delimiter[] = " \t\n";
	char *temp;
	char *wrd;
	listNode *cur = *head;
	while (cur->next)
	{
		cur = cur->next;
		*t_lines += cur->lines;
		for (i=0;i<cur->lines;i++)
		{
			*t_chars += strlen(cur->map[i]);							//count chars
			temp = malloc(sizeof(char)*(strlen(cur->map[i])+1));
			strncpy(temp,cur->map[i],strlen(cur->map[i]));
			temp[strlen(cur->map[i])] = '\0';
			wrd = strtok(temp,delimiter);
			*t_words += 1;
			while (wrd != NULL)											//count words
			{
				wrd = strtok(NULL, delimiter);
				if (wrd == NULL)
					continue;
				*t_words += 1;
			}
			free(temp);
		}
	}
}