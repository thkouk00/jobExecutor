#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "send_msg.h"
#include "list.h"
#include "map_file.h"
#include "trie.h"
#include "set_up_worker.h"
#include "maxcount.h"
#include "wc.h"
#include "search.h"

#define FIFO "/home/thanos/Desktop/fifo"
#define FIFO1 "/home/thanos/Desktop/fifo1"
#define FIFO2 "/home/thanos/Desktop/fifo2"
#define LOG "/home/thanos/Desktop/log/Worker_"
#define PERMS 0666

static volatile sig_atomic_t stop = 0;

void Usage(char *prog_name)			/* Usage */
{
   fprintf(stderr, "Usage: %s -d <docfile> -w <number of workers>\n", prog_name);
} 

void catchsig(int signo)
{
	if (signo == SIGCLD)
	{
		// while (waitpid(-1,NULL,WNOHANG)>0);
	}
	if (signo == SIGUSR2)
	{
		stop = 1;
		write(1, "Stop value changed", sizeof(char)*strlen("Stop value changed"));
	}
	if (signo == SIGALRM)
		write(1, "Alarm finished", sizeof(char)*strlen("Alarm finished"));
}

char *paths_to_pid; //keep track of paths , every cell has a pid

int main(int argc , char* argv[])
{
	int i,y;
	int W = 10;
	int doc_flag = 0;
	FILE *fp;

	if (argc < 2)
	{
		Usage(argv[0]);
		exit(1);
	}
	else		
	{
		for(i=1;i<argc;i+=2){
			if (!strcmp(argv[i],"-d"))
			{
				fp = fopen(argv[i+1], "r");
				doc_flag = 1;
			}
			else if (!strcmp(argv[i],"-w"))
			{
				W = atoi(argv[i+1]);
			}
		}
		if (!doc_flag){
			printf("Dockfile required\n");
			Usage(argv[0]);
			exit(1);
		}
	}
	int file_count = 0;
	char *buff = NULL;
	size_t buff_size;
	DIR *dp;
	struct dirent *entry;
	int lines = 0;
	int x , max_chars = 0;
	
	while ((x=getline(&buff,&buff_size,fp))>0)
	{
		if (x>max_chars)
			max_chars = x;
		if (buff[strlen(buff)-1] == '\n')		//axreiasto edw logika
			buff[strlen(buff)-1] = '\0';

		free(buff);
		buff = NULL;
		buff_size = 0;
		file_count = 0;
		lines++;
	};

	printf("Lines %d\n",lines);
	
	int temp_lines = lines;		
	int num_of_paths = 0 ;		//before sent msg to child, holds number of paths to sent
	fseek(fp, 0, SEEK_SET);
	if (W>lines)
		W = lines;
	int dirs_per_worker = lines/W;
	int modulo = lines%W;
	if (modulo != 0)
		dirs_per_worker++;
	int tmp_mod = modulo;
	printf("dirs_per_worker %d and workers %d\n",dirs_per_worker,W);
	// fclose(fp);

	static struct sigaction act;
	act.sa_handler = catchsig;
	// act.sa_handler = SIG_IGN;
	sigfillset(&(act.sa_mask));
	sigaction(SIGUSR1,&act,NULL);				//signal handler for SIGUSR1
	sigaction(SIGUSR2,&act,NULL);				//signal handler for SIGUSR2
	sigaction(SIGALRM,&act,NULL);
	sigaction(SIGCHLD,&act,NULL);

	int *pid_ar = malloc(sizeof(int)*W);
	int n = 0;

	char **name = malloc(sizeof(char*)*W);		//space to hold named pipes for every worker
	char **name2 = malloc(sizeof(char*)*W);		//space to hold named pipes for every worker
	for (i=0;i<W;i++)
	{						
		name[i] = malloc(sizeof(char)*(strlen(FIFO)+10));
		name2[i] = malloc(sizeof(char)*(strlen(FIFO)+12));
	}
	

	// create named piped for communication between parent-child
	if ((mkfifo(FIFO1,PERMS) < 0))				// read : parent <--> write : child
		perror("PARENT cant create FIFO1\n");
	

	int readfd , writefd , status;
	for (i=0;i<W;i++)
	{
		pid_ar[i] = fork();
		if (!pid_ar[i])					// child process 
		{
			time_t curtime;
			char *time_buff;		//take time for log file
			char *log_name = malloc(sizeof(char)*(strlen(LOG)+15));
			sprintf(log_name, "%s%d.txt",LOG,getpid());
			printf("LOG NAME %s\n", log_name);
			FILE *f = fopen(log_name, "a");
			if (f == NULL)
			{
				perror("Failed to open file\n");
				exit(1);
			}
			char *name = malloc(sizeof(char)*(strlen(FIFO)+10));		//for reading
			sprintf(name, "%s%d", FIFO,getpid());

			char *name2 = malloc(sizeof(char)*(strlen(FIFO)+12));		//for writing
			sprintf(name2, "%s_2", name);

			int num_of_paths;
			if ((mkfifo(name, PERMS)<0))							// create pipes
				perror("Error : creating FIFO\n");
			if ((mkfifo(name2, PERMS)<0))
				perror("Error : creating FIFO\n");
			
			while ((readfd = open(name, O_RDONLY|O_NONBLOCK))<0);
			while ((n=read(readfd, &num_of_paths, sizeof(int)))<=0); 		// read number of paths 
			
			//na to elegxw ti pairnei gia na mi skasei kapoia stigmi , giati to bazw kateuthian se 
			// metabliti (gia to read apo panw)

			char **path_array = malloc(sizeof(char*)*num_of_paths);
			char *tmp_buff = malloc(sizeof(char)*(max_chars));
			for (y=0;y<num_of_paths;y++)
			{
				while ((n=read(readfd,tmp_buff, sizeof(char)*(max_chars)))<=0);
				path_array[y] = malloc(sizeof(char)*(strlen(tmp_buff)+1));
				strncpy(path_array[y], tmp_buff , strlen(tmp_buff));
			}
			// close(readfd);
			free(tmp_buff);
			// one trie per Worker , one map per file
			//list to hold maps for every doc
			listNode **info = malloc(sizeof(listNode*)*num_of_paths);	
			trieNode_t *trie;
			CreateTrie(&trie);
			
			int *offset_array ;
			for (y=0;y<num_of_paths;y++)
			{
				set_up_worker(&info[y],path_array[y]);			//takes docfiles and map them
				fill_trie(&info[y], &trie);						//insert to trie
				printNode(&trie, "syspro");
				printNode(&trie, "to");

			}

			int size_to_read;
			tmp_buff = malloc(sizeof(char)*20); 	//buffer to hold number sent from parent
			
			//worker must wait for message from jobExecutor for queries
			
			while ((writefd = open(name2, O_WRONLY|O_NONBLOCK))<0);
			while(!stop)			//wait for SIGUSR2 to exit loop
			{
				pause(); 			//wait for SIGUSR1 to wake up and continue execution 
				
				// read query from jobExecutor
				while ((n=read(readfd,tmp_buff, sizeof(char)*20))<=0);
				size_to_read = atoi(tmp_buff);
				if (size_to_read != -1)
				{
					buff = malloc(sizeof(char)*(size_to_read)+1);	//htan size to read +1
					while ((n=read(readfd,buff, sizeof(char)*size_to_read))<=0);
					buff[size_to_read] = '\0';
					if (!strncmp(buff, "/search ", strlen("/search ")))
					{
						while ((n=read(readfd,tmp_buff, sizeof(char)*20))<=0);
						int deadline = atoi(tmp_buff);
						printf("RECEIVE %s and deadline %d\n", buff,deadline);
						search(&trie,buff,name2,writefd,f,deadline);
					}
					else if (!strncmp(buff, "/maxcount ", strlen("/maxcount ")) || !strncmp(buff, "/mincount ", strlen("/mincount ")))
					{
						maxcount(&trie,buff,name2,writefd,f);	
					}
					else if (!strncmp(buff, "/wc", strlen("/wc")))
					{
						int total[3]={0};
						int tmp_number , count = 0;
						char *buff1 = malloc(sizeof(char)*20); 		//temp size
						int temp_size;
						for (y=0;y<num_of_paths;y++)
							wc(&info[y],&total[0],&total[1],&total[2]);
						
						//send them to parent
						for (y=0;y<3;y++)
						{
							tmp_number = total[y];
							while (tmp_number != 0)			//length of number (digits)
							{
								tmp_number /= 10;
								++count;
							}
						}
						sprintf(buff1, "%d", count+4); 	//+4 is 3chars for | and one for end
						write(writefd, buff1, sizeof(char)*20);			//send it to parent
						free(buff1);
						buff1 = malloc(sizeof(char)*(count+3)); 
						sprintf(buff1, "%d|%d|%d",total[0],total[1],total[2]);
						printf("%s\n", buff1);
						write(writefd, buff1, sizeof(char)*(count+4)); 
						time(&curtime);
						time_buff = ctime(&curtime);
						time_buff[strlen(time_buff)-1] = '\0';
						printf("GRAFW LOGFILE\n");
						fprintf(f, "%s: wc: chars: %d words: %d lines: %d\n", time_buff,total[0],total[1],total[2]);
						printf("EGRAPSA\n");
						free(buff1);
					}


					free(buff);
					buff = NULL;
				}
				else
				{
					close(readfd);
					close(writefd);
					break;
				}
			};

			fclose(f);
			free(log_name);

			for (y=0;y<num_of_paths;y++)
			{
				free(path_array[y]);
				FreeList(&info[y]);
			}
			// fprintf(f, "ANTE RE MEGALE");
			free(path_array);
			free(pid_ar);
			free(name);
			fclose(fp);
			// fclose(f);
			// free(log_name);
			close(readfd);
			close(writefd);
			FreeTrie(&trie);		// eftiaxa to linfo
			exit(0);
		}
		else
		{	
			//send paths to workers

			send_msg(&fp,name[i],name2[i],max_chars,pid_ar[i],&modulo, &temp_lines,&dirs_per_worker,&tmp_mod);
		}
	}
	
	int *writefd_array = malloc(sizeof(int)*W);
	int *readfd_array = malloc(sizeof(int)*W);
	for (i=0;i<W;i++)								//pipes ready for use
	{
		while ((writefd_array[i] = open(name[i],O_WRONLY|O_NONBLOCK))<0);
		while ((readfd_array[i] = open(name2[i], O_RDONLY|O_NONBLOCK))<0);
	}

	// parent process
	while(1)
	{
		char *tmp_buff = malloc(sizeof(char)*20);		//buffer to hold number of chars to sent to child
		char *temp_buff2;								// helper buffer
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

		// printf("buff %s\ntemp %s\n", buff,temp);
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

	//end of program
	pid_t pid;
	for (i=0;i<W;i++)
	{
		pid = waitpid((pid_t)(-1),&status, 0) ;
		unlink(name[i]);
		free(name[i]);
		unlink(name2[i]);
		free(name2[i]);
		printf("Waited for %d\n",pid);
	}
	free(name);
	unlink(FIFO1);
	// unlink(FIFO2);
	fclose(fp);
	free(pid_ar);
	printf("~End of parent!~\n");
	return 0;
}
