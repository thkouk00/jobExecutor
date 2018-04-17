#include "search.h"

void search(trieNode_t **trie,char *buff , char *name2, int writefd,FILE *f)
{
	// int number = -1;
	char *word;
	char *buff1 = buff;
	char *size_buff = malloc(sizeof(char)*20);
	char *result = NULL;
	char delimiter[] = " \n\0";
	word = strtok(buff1,delimiter);
	printf("EINAI %s\n", word);
	word = strtok(NULL,delimiter);
	printf("EINAI %s\n", word);

	while (word != NULL)
	{	
		// word = strtok(NULL," \n\0");
		printf("WORD %s\n", word);
		find_word(trie, word, &result, NULL, 3);
		printf("RES is %s\n", result);
		if (result != NULL)
		{
			sprintf(size_buff, "%ld",strlen(result)+1);
			write(writefd, size_buff, sizeof(char)*20);
			write(writefd, result, sizeof(char)*(strlen(result)+1));
			// fprintf(f, "%s: search: %s ", time_buff);
			printf("SEARCH %s\n", result);
			free(result);
			result = NULL;
		}
		word = NULL;
		word = strtok(NULL,delimiter);
		printf("EINAI %s\n", word);

		if (word != NULL)
			write(writefd, "0", sizeof(char)*2);	//tell jobExecutor to continue receiving messages
	}
	printf("END\n");
	free(size_buff);
	write(writefd, "1", sizeof(char)*2);			//end
}