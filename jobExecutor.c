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
#include "worker.h"
#include "send_msg.h"
#include "user_query.h"
#include "list.h"
#include "map_file.h"
#include "trie.h"
#include "set_up_worker.h"
#include "maxcount.h"
#include "wc.h"
#include "search.h"

#define FIFO "/home/thanos/Desktop/fifo"
#define LOG "/home/thanos/Desktop/log/Worker_"
#define PERMS 0666

// static volatile sig_atomic_t triggered = 0;
static volatile sig_atomic_t stop = 0;
// static volatile sig_atomic_t end = 0;

void Usage(char *prog_name)			/* Usage */
{
   fprintf(stderr, "Usage: %s -d <docfile> -w <number of workers>\n", prog_name);
} 

void catchsig(int signo)
{
	if (signo == SIGCHLD)
	{
		while ((waitpid(-1,NULL,WNOHANG))>0);
	}
	if (signo == SIGALRM)
	{
		// write(1, "Alarm finished", sizeof(char)*strlen("Alarm finished"));
	}
	// if (signo == SIGINT)
	// {
	// 	// end = 1;
	// 	// write(1, "Alarm finished", sizeof(char)*strlen("Alarm finished"));
	// }
}

int *paths_to_pid; //keep track of paths , every cell has a pid

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
				if (fp == NULL)
				{
					perror("ERROR , cannot open file.\n");
					exit(1);
				}
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
	int **number_of_paths;
	while ((x=getline(&buff,&buff_size,fp))>0)
	{
		if (x>max_chars)
			max_chars = x;
		free(buff);
		buff = NULL;
		buff_size = 0;
		file_count = 0;
		lines++;
	};
	free(buff);							
	buff = NULL;
	
	int temp_lines = lines;		
	int num_of_paths = 0 ;				//before sent msg to child, holds number of paths to sent
	fseek(fp, 0, SEEK_SET);
	if (W>lines)
		W = lines;						//adjust workers to paths given
	int dirs_per_worker = lines/W;
	int modulo = lines%W;
	if (modulo != 0)
		dirs_per_worker++;
	int tmp_mod = modulo;
	
	static struct sigaction act;
	act.sa_handler = catchsig;
	sigfillset(&(act.sa_mask));
	sigaction(SIGUSR1,&act,NULL);				
	sigaction(SIGALRM,&act,NULL);
	sigaction(SIGCHLD,&act,NULL);
	// sigaction(SIGINT,&act,NULL);
	// sigaction(SIGUSR2,&act,NULL);				

	paths_to_pid = malloc(sizeof(int)*lines);
	for (i=0;i<lines;i++)
		paths_to_pid[i] = -1; 
	int *pid_ar = malloc(sizeof(int)*W);
	int n = 0;

	char **name = malloc(sizeof(char*)*W);		//space to hold named pipes for every worker
	char **name2 = malloc(sizeof(char*)*W);		//space to hold named pipes for every worker
	for (i=0;i<W;i++)
	{						
		name[i] = malloc(sizeof(char)*(strlen(FIFO)+10));
		name2[i] = malloc(sizeof(char)*(strlen(FIFO)+12));
	}
	
	int status;
	for (i=0;i<W;i++)
	{
		pid_ar[i] = fork();
		if (!pid_ar[i])					// child process 
		{
			for (int y=0;y<W;y++)		//malloc'd before fork , so i free them
			{
				free(name[y]);
				free(name2[y]);
			}
			free(name);
			free(name2);

			worker(max_chars,fp,pid_ar);		// worker's job
		}
		else
		{	
			//send paths to workers
			send_msg(&fp,name[i],name2[i],max_chars,pid_ar[i],paths_to_pid,&modulo, &temp_lines,&dirs_per_worker,&tmp_mod,lines);
		}
	}
	
	int *writefd_array = malloc(sizeof(int)*W);
	int *readfd_array = malloc(sizeof(int)*W);
	for (i=0;i<W;i++)								//pipes ready for use
	{
		while ((writefd_array[i] = open(name[i],O_WRONLY|O_NONBLOCK))<0)
			usleep(2000);
		while ((readfd_array[i] = open(name2[i], O_RDONLY|O_NONBLOCK))<0)
			usleep(2000);
	}

	// parent process
	// take user queries and route them to workers
	user_query(W,pid_ar,readfd_array,writefd_array,paths_to_pid,name,name2,lines,max_chars,fp);

	//end of program
	free(paths_to_pid);
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
	free(name2);
	fclose(fp);
	free(pid_ar);
	printf("~End of jobExecutor!~\n");
	return 0;
}
