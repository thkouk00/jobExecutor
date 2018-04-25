#include "user_query.h"

void catchkillsg(int signo)
{
	if (signo == SIGINT)
	{
		end = 1;
		write(1, "HERE\n", sizeof(char)*strlen("HERE\n"));
	}
}

void user_query(int W,int *pid_ar,int *readfd_array , int *writefd_array,int *paths_to_pid,char **name,char **name2,int lines,int max_chars,FILE *fp)
{
	static struct sigaction action;
	action.sa_handler = catchkillsg;
	sigfillset(&(action.sa_mask));
	sigaction(SIGINT,&action,NULL);
	int wrong_input = 0;
	while(1)
	{
		int x , n , i;
		char *tmp_buff = malloc(sizeof(char)*20);		//buffer to hold number of chars to sent to child
		char *temp_buff2;								// helper buffer
		char *buff = NULL;
		size_t buff_size;
		int old_pid , kill_flag = 0;
		int path_count=0;
		printf("END is %d\n", end);
		// if (end)
		// {
		// 	printf("MPIKA\n");
		// 	for (i=0;i<W;i++)
		// 	{
		// 		while ((n=read(readfd_array[i],tmp_buff, sizeof(char)*2))<=0)
		// 			usleep(2000);
		// 		kill(pid_ar[i],SIGUSR1);
		// 		write(writefd_array[i], "-1", sizeof(char)*20);		//send -1 to stop child's loop
		// 	}
		// 	break;
		// }
		if (!wrong_input)
		{
			// check if SIGTERM raised for child , if yes replace child
			for (i=0;i<W;i++)
			{
				while ((n=read(readfd_array[i],tmp_buff, sizeof(char)*2))<=0)
					usleep(2000);
				if (!strcmp(tmp_buff,"~"))		//child must die
				{
					// free memory from killed child
					// send message to child to end
					kill(pid_ar[i],SIGUSR1);							
					write(writefd_array[i], "-1", sizeof(char)*20);		//send -1 to stop child's loop
					close(writefd_array[i]);
					close(readfd_array[i]);	
					
					old_pid = pid_ar[i];
					
					pid_ar[i] = fork();					//fork new child
					if (!pid_ar[i])
					{
						for (int y=0;y<W;y++)	//malloc'd before fork , so i free them
						{
							free(name[y]);
							free(name2[y]);
						}
						free(name);
						free(name2);
						free(tmp_buff);
						free(paths_to_pid);
						free(readfd_array);
						free(writefd_array);
						//start new worker
						worker(max_chars, fp, pid_ar);
					}
					else
					{
						//replace old workers info 
						path_count = 0;
						for (int j=0;j<lines;j++)
						{
							if (paths_to_pid[j] == old_pid)
							{
								paths_to_pid[j] = pid_ar[i];
								path_count++;
							}
						}
						unlink(name[i]);
						unlink(name2[i]);
						sprintf(name[i], "%s%d", FIFO,pid_ar[i]);
						sprintf(name2[i], "%s_2",name[i]);	
						while ((writefd_array[i] = open(name[i],O_WRONLY|O_NONBLOCK))<0)
							usleep(2000);
						while ((readfd_array[i] = open(name2[i], O_RDONLY|O_NONBLOCK))<0)
							usleep(2000);
						write(writefd_array[i], &path_count, sizeof(int));
						fseek(fp, 0, SEEK_SET);
						for (int j=0;j<lines;j++)		//send paths to new worker
						{
							getline(&buff,&buff_size,fp);
							if (paths_to_pid[j] == pid_ar[i])
							{
								buff[strlen(buff)-1] = '\0';			
								write(writefd_array[i], buff, sizeof(char)*max_chars); 
							}
							free(buff);
							buff = NULL;
						}	
						free(buff);
						buff = NULL;
						fseek(fp, 0, SEEK_SET);
						kill_flag = 1;
						while ((n=read(readfd_array[i],tmp_buff, sizeof(char)*2))<=0)
							usleep(2000);
					}
				}
				else if (!strcmp(tmp_buff,"^"))		//child is alive
				{
					continue;
				}
			}
		}
		
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

		// printf("EDW END %d\n", end);
		// if (end)
		// {
		// 	printf("MPIKA\n");
		// 	for (i=0;i<W;i++)
		// 	{
		// 		// while ((n=read(readfd_array[i],tmp_buff, sizeof(char)*2))<=0)
		// 		// 	usleep(2000);
		// 		kill(pid_ar[i],SIGUSR1);
		// 		write(writefd_array[i], "-1", sizeof(char)*20);		//send -1 to stop child's loop
		// 	}
		// 	break;
		// }

		// check if query is valid
		// if yes , send query to workers and wait for answer
		// else user must try again
		if (!strncmp(txt, "/search", strlen("/search")+1) && strlen(buff)>13)
		{
			int cnt = 0;
			char *word2;
			deadline = -1;
			temp_buff2 = malloc(sizeof(char)*strlen(buff));
			strcpy(temp_buff2, buff);
			word = strtok(buff, " \0\n");
			while (word != NULL)				// check input for -d N 
			{
				cnt++;
				word = strtok(NULL, " \0\n");
			}	
			strcpy(buff, temp_buff2);
			word = strtok(buff, " \0\n");
			int loop = 0;
			while (word != NULL)				// check input for -d N 
			{
				loop++;
				if (loop == cnt-1)
				{
					if (!strcmp(word,"-d"))
					{
						word = strtok(NULL, " \0\n");
						deadline = atoi(word);
					}
					else
						break;
				}
				word = strtok(NULL, " \0\n");
			}
			if (deadline > 0)
			{	
				printf("DEADLINE %d\n", deadline);
				valid = 1;
				sprintf(tmp_buff, "%d",deadline);
				memset(buff, 0, sizeof(char)*buff_size);
				strncpy(buff, temp_buff2, strlen(temp_buff2)-strlen(tmp_buff)-4);
			}
			else
			{
				valid = 0;
				wrong_input = 1;
				printf("Wrong input try again\n");
			}
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
		{
			wrong_input = 1;
			printf("Wrong input try again\n");
		}
		if (valid)
		{
			wrong_input = 0;
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
					
					while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0)
						usleep(2000);
					word = strtok(tmp_buff, " \0\n");
					int length = atoi(word);
					if (length != -1)
					{
						temp_buff2 = malloc(sizeof(char)*length); 
						while ((n=read(readfd_array[j],temp_buff2, sizeof(char)*length))<=0)
							usleep(2000);
						results[j] = malloc(sizeof(char)*length);
						strcpy(results[j], temp_buff2);
						free(temp_buff2);
					}
					else			
					{
						//no response till deadline , ignore answer
						results[j] = NULL;
						workers_failed--;
					}
				}
				// de-serialize answer
				// one message for every query from every worker
				// structure of msg: file|offset0|offset1|...|offsetN|$|...
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
					
					while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0)
						usleep(2000);

					word = strtok(tmp_buff," |\0\n");
					int length = atoi(word);			//mikos string poy perimenw
					word = strtok(NULL," |\0\n");
					valid = atoi(word);					//kodikos valid
					
					if (length>0)
					{	
						int t_num = 0;
						
						temp_buff2 = malloc(sizeof(char)*(length));
						while ((n=read(readfd_array[j],temp_buff2, sizeof(char)*length))<=0)
							usleep(2000);
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
					while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0)
						usleep(2000);
					int length = atoi(tmp_buff);
					temp_buff2 = malloc(sizeof(char)*length);
					while ((n=read(readfd_array[j],temp_buff2, sizeof(char)*(length)))<=0)
						usleep(2000);
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