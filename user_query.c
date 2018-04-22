#include "user_query.h"

void user_query(int W,int *pid_ar,int *readfd_array , int *writefd_array)
{
	while(1)
	{
		int x , n;
		char *tmp_buff = malloc(sizeof(char)*20);		//buffer to hold number of chars to sent to child
		char *temp_buff2;								// helper buffer
		char *buff = NULL;
		size_t buff_size;
		printf("Give input:\n");
		while ((x=getline(&buff,&buff_size,stdin))<=0);
		char delimiter[] = " \t\n";
		char *temp = malloc(sizeof(char)*(strlen(buff)+1));
		strncpy(temp, buff,strlen(buff));
		char *txt = strtok(temp,delimiter);
		char *word; // use for strtok 
		int valid = 0;
		int exit_flag = 0;
		int final_num; 					//used in max/mincount
		int deadline = -1;				//used for search

		if (!strncmp(txt, "/search", strlen("/search")+1))
		{
			char *word2;
			deadline = -1;
			temp_buff2 = malloc(sizeof(char)*strlen(buff));
			strcpy(temp_buff2, buff);
			word = strtok(buff, " \0\n");
			while (word != NULL)				// check input for -d N 
			{
				if (!strcmp(word,"-d"))
				{

					word = strtok(NULL, " \0\n");
					if (word == NULL)
						break;
					word2 = strtok(NULL, " \0\n");
					if (word2 == NULL)
					{
						deadline = atoi(word);
						break;
					}
					else
					{
						word = word2;
					}
				}
				word = strtok(NULL, " \0\n");
			}	
			if (deadline > 0)
			{	
				valid = 1;
				sprintf(tmp_buff, "%d",deadline);
				memset(buff, 0, sizeof(char)*buff_size);
				strncpy(buff, temp_buff2, strlen(temp_buff2)-strlen(tmp_buff)-4);
			}
			else
				printf("Wrong input try again\n");
			free(temp_buff2);
		}
		else if (!strncmp(txt, "/maxcount", strlen("/maxcount")+1))
			valid = 2;
		else if (!strncmp(txt, "/mincount", strlen("/mincount")+1))
			valid = 3;
		else if (!strncmp(txt, "/wc", strlen("/wc")+1))
			valid = 4;
		else if (!strncmp(txt, "/exit", strlen("/exit")+1))
		{
			printf("Exiting\n");
			for (int j=0;j<W;j++)
			{	
				kill(pid_ar[j],SIGUSR1);

				//send -1 to stop child's loop
				write(writefd_array[j], "-1", sizeof(char)*20);
				close(writefd_array[j]);
				close(readfd_array[j]);		//den to eixa katholoy
			}
			free(writefd_array);			//oute ayta ta xa
			free(readfd_array);
			free(tmp_buff);
			free(buff);						//extra kai ayto
			break;
		}
		else
			printf("Wrong input try again\n");
		if (valid)
		{
			
			// sprintf(tmp_buff, "%ld",strlen(buff));
			txt = NULL;
			for (int j=0;j<W;j++)				//send query to workers
			{	
				sprintf(tmp_buff, "%ld",strlen(buff));
				kill(pid_ar[j],SIGUSR1);
				
				//inform child how many chars to expect
				write(writefd_array[j], tmp_buff, sizeof(char)*20);		
				//send query
				write(writefd_array[j], buff, sizeof(char)*(strlen(buff)));
				if (valid == 1)
				{	
					sprintf(tmp_buff, "%d",deadline);
					write(writefd_array[j], tmp_buff, sizeof(char)*20);
				}
			}
			if (valid == 1)
			{	
				FILE *fpointer;
				int workers_failed = W;
				int stop;
				int offset;
				char *line_buff = NULL;
				size_t s;
				char **results = malloc(sizeof(char*)*W);
				for (int j=0;j<W;j++)
				{
					
					while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0);
					word = strtok(tmp_buff, " \0\n");
					int length = atoi(word);
					if (length != -1)
					{
						temp_buff2 = malloc(sizeof(char)*length); 
						while ((n=read(readfd_array[j],temp_buff2, sizeof(char)*length))<=0);
						results[j] = malloc(sizeof(char)*length);
						strcpy(results[j], temp_buff2);
						free(temp_buff2);
					}
					else
					{
						results[j] = NULL;
						workers_failed--;
					}
				}
				for (int j=0;j<W;j++)
				{
					if (results[j] == NULL)
						continue;
					word = strtok(results[j],"|");	
					fpointer = fopen(word,"r");
					word = strtok(NULL,"|");
					while (word != NULL)
					{
						if (!strcmp(word,"$"))
						{
							word = strtok(NULL,"|");
							fclose(fpointer);
							if (word == NULL)
								break;
							else
								fpointer = fopen(word,"r");
						}
						else
						{
							offset = atoi(word);
							fseek(fpointer,offset,SEEK_SET);
							getline(&line_buff,&s,fpointer);
							printf("%s\n", line_buff);
							free(line_buff);
							line_buff = NULL;
						}
						word = strtok(NULL,"|");
					}
				}
				printf("Result from %d/%d workers!\n", workers_failed,W);
				
				for (int j=0;j<W;j++)
				{
					if (results[j]!=NULL)
						free(results[j]);
				}
				free(results);
			}
			else if (valid == 2 || valid == 3)		//for max-mincount
			{
				if (valid == 2)
					final_num = 0;
				else if (valid == 3)
					final_num = 999999;
				for (int j=0;j<W;j++)
				{
					memset(tmp_buff, 0, sizeof(char)*20);  //reset memory space
					
					while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0);

					word = strtok(tmp_buff," |\0\n");
					int length = atoi(word);			//mikos string poy perimenw
					word = strtok(NULL," |\0\n");
					valid = atoi(word);					//kodikos valid
					
					if (length>0)
					{	
						int t_num = 0;
						
						temp_buff2 = malloc(sizeof(char)*(length));
						while ((n=read(readfd_array[j],temp_buff2, sizeof(char)*length))<=0);
						word = strtok(temp_buff2,"|\0\n");
						t_num = atoi(word);
						if (valid == 2 && t_num > final_num)
						{
							final_num = t_num; 
							word = strtok(NULL,"|\0\n");
							if (txt!=NULL)
								free(txt);
							txt = malloc(sizeof(char)*(strlen(word)+1));
							strncpy(txt, word,strlen(word)+1);
						}
						else if (valid == 3 && t_num < final_num)
						{
							final_num = t_num; 
							word = strtok(NULL,"|\0\n");
							if (txt!=NULL)
								free(txt);
							txt = malloc(sizeof(char)*(strlen(word)+1));
							strncpy(txt, word,strlen(word)+1);
						}
						else if (t_num == final_num)
						{
							word = strtok(NULL,"|\0\n");
							if (strcmp(txt, word)>0)
							{
								free(txt);
								txt = malloc(sizeof(char)*(strlen(word)+1));
								strncpy(txt, word,strlen(word)+1);
							}
						}
						free(temp_buff2);
					
					}
					
					if (j == W-1)
					{
						if (txt!=NULL)
						{
							printf("Docfile is %s with %d times.\n", txt,final_num);
							free(txt);
						}
						else
							printf("No files found containing requested word.\n");
					}
				}	
			}
			else if (valid == 4)
			{
				int total_stats[3] = {0};
				for (int j=0;j<W;j++)
				{
					while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0);
					int length = atoi(tmp_buff);
					temp_buff2 = malloc(sizeof(char)*length);
					while ((n=read(readfd_array[j],temp_buff2, sizeof(char)*(length)))<=0);
					word = strtok(temp_buff2, "|\n\0");
					total_stats[0] += atoi(word);
					for (int k=1;k<3;k++)
					{
						word = strtok(NULL, "|\n\0");
						total_stats[k] += atoi(word);
					}
					free(temp_buff2);
				}
				printf("WC results:\n");
				printf("Total chars : %d\n", total_stats[0]);
				printf("Total words : %d\n", total_stats[1]);
				printf("Total lines : %d\n", total_stats[2]);
			}	
			
		}
		free(temp);
		free(buff);
		buff = NULL;		
	};
}