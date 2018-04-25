#include "search.h"

static volatile sig_atomic_t stop = 0;

void catchsigd(int signo)
{
	if (signo == SIGALRM)
	{
		// write(1, "**ALARM**\n",sizeof(char)*strlen("**ALARM**\n"));
		stop = 1;
	}
}

void search(trieNode_t **trie,char *buff , char *name2, int writefd,FILE *f,int deadline)
{

	time_t curtime;
	char *time_buff;
	static struct sigaction act;
	act.sa_handler = catchsigd;
	sigfillset(&(act.sa_mask));
	sigaction(SIGALRM,&act,NULL);

	size_t total_size = 100;
	char *word , *word2 , *next1 , *next2;		//used for strtok_r 
	char *size_buff = malloc(sizeof(char)*20);
	char *result = NULL;
	char *total_res = malloc(sizeof(char)*total_size);
	memset(total_res, 0, sizeof(char)*total_size);
	char delimiter[] = " \n\0";
	word = strtok_r(buff,delimiter,&next1);					
	word = strtok_r(NULL,delimiter,&next1);
	
	
	alarm(deadline);
	// if (getpid()%2 == 0)						// to check that sigalarm works
	// 	sleep(deadline);
	while (word != NULL)
	{	
		find_word(trie, word, &result, NULL, 3);
		if (result != NULL)
		{
			if (strlen(total_res)+strlen(result)+strlen(word) >= total_size)
			{	
				total_size += strlen(result)+strlen(word)+4;
				total_res = realloc(total_res, total_size);
			}
			sprintf(total_res+strlen(total_res), "%s", result);

			word2 = strtok_r(result, "|",&next2);			//path
			time(&curtime);
			time_buff = ctime(&curtime);
			time_buff[strlen(time_buff)-1] = '\0';
			fprintf(f, "%s: search: %s path: %s", time_buff,word,word2);
			while (word2 != NULL)
			{
				if (!strcmp(word2, "$"))
				{
					word2 = strtok_r(NULL, "|",&next2);
					if (word2 != NULL)
						fprintf(f, " : %s", word2);
					else 
						break;
				}
				word2 = strtok_r(NULL, "|",&next2);
			}
			fprintf(f, "\n");
			free(result);
			result = NULL;
		}
		else
		{
			time(&curtime);
			time_buff = ctime(&curtime);
			time_buff[strlen(time_buff)-1] = '\0';
			fprintf(f, "%s: search: %s path: NONE\n", time_buff,word);
		}
		word = strtok_r(NULL,delimiter,&next1);
	}
	alarm(0);
	
	if (stop)
	{
		sprintf(size_buff, "-1");
		write(writefd, size_buff, sizeof(char)*20);
	}
	else
	{
		sprintf(size_buff, "%ld",strlen(total_res)+1);
		write(writefd, size_buff, sizeof(char)*20);
		write(writefd, total_res, sizeof(char)*(strlen(total_res)+1));
	}
	free(size_buff);
	free(total_res);
}