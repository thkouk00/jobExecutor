#include "maxcount.h"	

void maxcount(trieNode_t **trie,char *buff , char *name2, int writefd,FILE *f)
{
	int flag;
	char *word;
	char *buff1 = buff;
	char *time_buff;
	time_t curtime;
	word = strtok(buff1," \n\0");
	if (!strcmp("/maxcount",word))
		flag = 0;
	else
		flag = 1;
	word = strtok(NULL," \n\0");
	
	char *docname = NULL;
	int number;
	
	find_word(trie, word,&docname,&number,flag);

	buff1 = malloc(sizeof(char)*20);
	if (!docname)
	{
		number = 0;
		sprintf(buff1, "-1|%s","2");
		//stelnei to -1 san mikos string kai timi valid
		write(writefd, buff1, sizeof(char)*20);
	}
	else
	{	
		if (!flag)
			sprintf(buff1, "%ld|%s",sizeof(char)*(strlen(docname)+10),"2");		//maxcount
		else
			sprintf(buff1, "%ld|%s",sizeof(char)*(strlen(docname)+10),"3");		//mincount
		
		//stelnei mikos string kai timi gia valid
		write(writefd, buff1, sizeof(char)*20);
		free(buff1);
		
		buff1 = malloc(sizeof(char)*(strlen(docname)+10));
		sprintf(buff1, "%d|%s",number,docname);
		//stelnei ton arithmo emfanisewn kai to onoma tou arxeiou
		write(writefd, buff1, sizeof(char)*(strlen(docname)+10));
		time(&curtime);
		time_buff = ctime(&curtime);
		time_buff[strlen(time_buff)-1] = '\0';
		if (!flag)
			fprintf(f, "%s: maxcount : word: %s document:%s number:%d\n", time_buff,word,docname,number);
		else
			fprintf(f, "%s: mincount : word: %s document:%s number:%d\n", time_buff,word,docname,number);
		if (docname)
			free(docname);
	}
	free(buff1);
}