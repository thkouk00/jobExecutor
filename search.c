#include "search.h"

void search(trieNode_t **trie,char *buff , char *name2, int writefd,FILE *f)
{
	char *word;
	char *buff1 = buff;
	char *size_buff = malloc(sizeof(char)*20);
	char *result = NULL;
	char delimiter[] = " \n\0";
	
	word = strtok(buff1,delimiter);					//htan buff1
	word = strtok(NULL,delimiter);
	while (word != NULL)
	{	
		find_word(trie, word, &result, NULL, 3);
		if (result != NULL)
		{
			sprintf(size_buff, "%ld",strlen(result)+1);
			write(writefd, size_buff, sizeof(char)*20);
			write(writefd, result, sizeof(char)*(strlen(result)+1));
			// fprintf(f, "%s: search: %s ", time_buff);
			free(result);
			result = NULL;
		}
		else
		{
			sprintf(size_buff, "-1");						//inform jobExecutor not to wait message
			write(writefd, size_buff, sizeof(char)*20);
		}
		word = strtok(NULL,delimiter);
		
		if (word != NULL)
			write(writefd, "0", sizeof(char)*2);	//tell jobExecutor to continue receiving messages
		else
			write(writefd, "1", sizeof(char)*2);	// no more messages to send
	}
	free(size_buff);
}