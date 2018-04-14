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
#include "send_msg.h"
#include "list.h"
#include "map_file.h"
#include "trie.h"

#define FIFO "/home/thanos/Desktop/fifo"
#define FIFO1 "/home/thanos/Desktop/fifo1"
#define FIFO2 "/home/thanos/Desktop/fifo2"
#define PERMS 0666

static volatile sig_atomic_t stop = 0;

void Usage(char *prog_name)			/* Usage */
{
   fprintf(stderr, "Usage: %s -d <docfile> -w <number of workers>\n", prog_name);
} 

void catchsig(int signo)
{
	if (signo == SIGUSR2)
	{
		stop = 1;
		write(1, "Stop value changed", sizeof(char)*strlen("**Stop value changed"));
	}
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
			char *name = malloc(sizeof(char)*(strlen(FIFO)+10));		//for reading
			sprintf(name, "%s%d", FIFO,getpid());

			char *name2 = malloc(sizeof(char)*(strlen(FIFO)+12));		//for writing
			sprintf(name2, "%s_2", name);

			int num_of_paths;
			if ((mkfifo(name, PERMS)<0))
				perror("Error : creating FIFO\n");
			if ((mkfifo(name2, PERMS)<0))
				perror("Error : creating FIFO\n");
			
			while ((readfd = open(name, O_RDONLY|O_NONBLOCK))<0);
			while ((n=read(readfd, &num_of_paths, sizeof(int)))<=0); 		// read number of paths 
			
			char **path_array = malloc(sizeof(char*)*num_of_paths);
			char *tmp_buff = malloc(sizeof(char)*(max_chars));
			for (y=0;y<num_of_paths;y++)
			{
				while ((n=read(readfd,tmp_buff, sizeof(char)*(max_chars)))<=0);
				path_array[y] = malloc(sizeof(char)*(strlen(tmp_buff)+1));
				strncpy(path_array[y], tmp_buff , strlen(tmp_buff));
			}
			close(readfd);
			free(tmp_buff);
			// one trie per Worker , one map per file
			FILE *file;
			int count_lines = 0;
			int max = 0;
			//list to hold maps and tries for every doc
			listNode **info = malloc(sizeof(listNode*)*num_of_paths);	
			trieNode_t *trie;
			CreateTrie(&trie);
			
			int *offset_array ;
			for (y=0;y<num_of_paths;y++)
			{
				dp = opendir(path_array[y]);
				if (dp == NULL)
					fprintf(stderr, "ERROR in open directory\n");
				printf("%s:\n",path_array[y]);
				while ((entry = readdir(dp)) != NULL)
				{

					if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
						continue;
					char *filename = malloc(sizeof(char)*(strlen(path_array[y])+strlen(entry->d_name)+2));
					sprintf(filename, "%s/%s",path_array[y],entry->d_name);
					
					file = fopen(filename, "r");
					max = 0;
					count_lines = 0;
					int tmp_size = 20;
					int pos = 1;
					offset_array = malloc(sizeof(int)*tmp_size);
					offset_array[0] = 0;
					int count = 0;
					while ((x=getline(&buff,&buff_size,file))>0)
					{
						count+=x;
						if (pos-1 < tmp_size)
						{
							offset_array[pos] = count;
							pos++;
						}
						else
						{
							tmp_size +=tmp_size;
							offset_array = realloc(offset_array, sizeof(int)*(tmp_size));
							offset_array[pos] = count;
							pos++;
						}
						if (x>max)
							max = x;
						count_lines++;
						free(buff);
						buff = NULL;
						buff_size = 0;
					}
					fseek(file, 0, SEEK_SET);
					pos--;	 // arxika htan katw apo to insert
					insert(&info[y],filename,count_lines,max,offset_array,pos);	
					map_file(file,&info[y],filename);				//function to map file
					fclose(file);
					free(filename);
				}
				closedir(dp);

				//eisagogi sto trie
				fill_trie(&info[y], &trie);
				// printNode(&trie, "syspro");
				printNode(&trie, "to");

			}

			int size_to_read;
			tmp_buff = malloc(sizeof(char)*20); 	//buffer to hold number sent from parent
			
			//worker must wait for message from jobExecutor for queries
			
			while ((readfd = open(name, O_RDONLY|O_NONBLOCK))<0);
			while(!stop)			//wait for SIGUSR2 to exit loop
			{
				pause(); 			//wait for SIGUSR1 to wake up and continue execution 
				
				// read query from jobExecutor
				while ((n=read(readfd,tmp_buff, sizeof(char)*20))<=0);
				size_to_read = atoi(tmp_buff);
				if (size_to_read != -1)
				{
					buff = malloc(sizeof(char)*(size_to_read+1));
					while ((n=read(readfd,buff, sizeof(char)*size_to_read))<=0);
					if (!strncmp(buff, "/search ", strlen("/search ")))
						;//go to search
					else if (!strncmp(buff, "/maxcount ", strlen("/maxcount ")) || !strncmp(buff, "/mincount ", strlen("/mincount ")))
					{
						int flag;
						char *word;
						char *buff1 = buff ;//= malloc(sizeof(char)*(strlen(buff)-strlen("/maxcount ")));
						word = strtok(buff1," \n\0");
						if (!strcmp("/maxcount",word))
							flag = 0;
						else
							flag = 1;
						word = strtok(NULL," \n\0");
						
						printf("BUFF1 is %s\n", word);
						char *docname = NULL;
						int number;
						
						find_word(&trie, word,&docname,&number,flag);
						// while (( = open(name,O_WRONLY|O_NONBLOCK))<0);
						if (!docname)
						{
							number = 0;
							buff1 = malloc(sizeof(char)*12);
							sprintf(buff1, "-1|%s","2");
							write(writefd, buff1, sizeof(char)*20);
							free(buff1);
						}
						else
						{
							printf("Chose doc is %s with %d\n", docname,number);
							//return it to jobExecutor
							buff1 = malloc(sizeof(char)*12);
							//na dw meta gia to "2" giati tha pianei kai to mincount
							sprintf(buff1, "%ld|%s",sizeof(char)*(strlen(docname)+10),"2");

							printf("BUFF1 %s\n", buff1);
							while ((writefd = open(name2,O_WRONLY|O_NONBLOCK))<0);
							write(writefd, buff1, sizeof(char)*20);
							free(buff1);
							
							buff1 = malloc(sizeof(char)*(strlen(docname)+10));
							sprintf(buff1, "%d|%s",number,docname);
							// write(writefd, buff1, sizeof(char)*strlen(buff1));
							write(writefd, buff1, sizeof(char)*(strlen(docname)+10));
							printf("WRITE %s\n", buff1);
							if (docname)
								free(docname);
							free(buff1);
						}
					}
					// else if (!strncmp(buff, "/mincount ", strlen("/mincount ")))
					// {
					// 	find_word(&trie, word,&docname,&number,1);
					// }
					else if (!strncmp(buff, "/wc ", strlen("/wc ")))
						;// go

					
					free(buff);
				}
				else
				{
					close(readfd);
					break;
				}
			};
			printf("OUT of loop %d\n", getpid());
			// close(readfd);
			//kill(getpid(),SIGUSR1);
			for (y=0;y<num_of_paths;y++)
				free(path_array[y]);
			free(path_array);
			free(pid_ar);
			free(name);
			fclose(fp);
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
	// parent process
	while(1)
	{
		char *tmp_buff = malloc(sizeof(char)*20);		//buffer to hold number of chars to sent to child
		printf("Give input:\n");
		while ((x=getline(&buff,&buff_size,stdin))<=0);
		char delimiter[] = " \t\n";
		char *temp = malloc(sizeof(char)*(strlen(buff)+1));
		strncpy(temp, buff,strlen(buff));
		char *txt = strtok(temp,delimiter);
		int valid = 0;
		int exit_flag = 0;
		int final_num = 0; 
		// printf("buff %s\ntemp %s\n", buff,temp);
		if (!strncmp(txt, "/search", strlen("/search")+1))
			valid = 1;
		else if (!strncmp(txt, "/maxcount", strlen("/maxcount")+1))
		{
			valid = 2;
		}
		else if (!strncmp(txt, "/mincount", strlen("/mincount")+1))
		{
			valid = 3;
		}
		else if (!strncmp(txt, "/wc", strlen("/wc")+1))
			valid = 4;
		else if (!strncmp(txt, "/exit", strlen("/exit")+1))
		{
			printf("Exiting\n");
			for (int j=0;j<W;j++)
			{	
				kill(pid_ar[j],SIGUSR1);
				printf("SIGUSR2 %d\n",pid_ar[j]);
				write(writefd_array[j], "-1", sizeof(char)*20);
				// write(writefd, "-1", sizeof(char)*20);
				close(writefd_array[j]);
				// close(writefd);
			}
			break;
		}
		else
			printf("Wrong input try again\n");
		if (valid)
		{
			sprintf(tmp_buff, "%ld",strlen(buff));
			printf("tmp_buff is %s\n", tmp_buff);
			txt = NULL;
			for (int j=0;j<W;j++)
			{	
				kill(pid_ar[j],SIGUSR1);
				printf("SIGUSR1 %d\n",pid_ar[j]);
				while ((writefd_array[j] = open(name[j],O_WRONLY|O_NONBLOCK))<0);
				write(writefd_array[j], tmp_buff, sizeof(char)*20);		//inform child how many chars to expect
				write(writefd_array[j], buff, sizeof(char)*(strlen(buff)));
				
				printf("name->> %s\n", name2[j]);
				memset(tmp_buff, 0, sizeof(char)*20);
				printf("tmp_buff is %s\n", tmp_buff);
				while ((readfd_array[j] = open(name2[j], O_RDONLY|O_NONBLOCK))<0);
				while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*20))<=0);
				// while ((readfd = open(FIFO1, O_RDONLY|O_NONBLOCK))<0);
				// while ((n=read(readfd,tmp_buff, sizeof(char)*20))<=0);

				char *word;
				word = strtok(tmp_buff," |\0\n");
				int length = atoi(word);
				word = strtok(NULL," |\0\n");
				valid = atoi(word);
				printf("VALID IS %d and length %d\n", valid,length);
				if (valid == 1)
					;
				else if (valid == 2)
				{
					if (length>0)
					{	
						int t_num = 0;
						
						free(tmp_buff);
						tmp_buff = malloc(sizeof(char)*(length));
						while ((n=read(readfd_array[j],tmp_buff, sizeof(char)*length))<=0);
						word = strtok(tmp_buff,"|\0\n");
						t_num = atoi(word);
						if (t_num > final_num)
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
							if (strcmp(txt, word)<0)
							{
								free(txt);
								txt = malloc(sizeof(char)*(strlen(word)+1));
								strncpy(txt, word,strlen(word)+1);
							}
						}

						free(tmp_buff);
						if (j == W-1)
						{
							if (txt!=NULL)
							{
								printf("Docfile is %s with %d times.\n", txt,final_num);
								free(txt);
							}
							else
								printf("No files found\n");
						}
					
					}
					else
						printf("Word not found\n");
				}
				else if (valid == 3)
					;
				else if (valid == 4)
					;
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
