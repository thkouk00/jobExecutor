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

#define FIFO "/home/thanos/Desktop/fifo"
#define FIFO1 "/home/thanos/Desktop/fifo1"
#define FIFO2 "/home/thanos/Desktop/fifo2"
#define PERMS 0666

typedef struct Message
{
	pid_t pid;
	char **path_table;	
}message;



void Usage(char *prog_name)			/* Usage */
{
   fprintf(stderr, "Usage: %s -d <docfile> -w <number of workers>\n", prog_name);
} 

void catchchild(int signo)
{
	pid_t pid ;
	int status;
	
	printf("Catching: signo=%d and pid %d\n",signo,pid);
	printf("Catching: returning\n");
}

char *array; //keep track of paths , every cell has a pid

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
		if (buff[strlen(buff)-1] == '\n')
			buff[strlen(buff)-1] = '\0';

		// strncpy(buff, buff, strlen(buff));
		printf("%s.\n", buff);
		// dp = opendir(buff);
		// if (dp == NULL)
		// 	fprintf(stderr, "ERROR\n");
		// while ((entry = readdir(dp)) != NULL)
		// {
		// 	if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
		// 		continue;
		// 	printf("%s\n", entry->d_name);
		// 	file_count++;
		// }
		free(buff);
		buff = NULL;
		buff_size = 0;
		// closedir(dp);
		printf("COunt %d\n", file_count);
		file_count = 0;
		lines++;
	};

	printf("Lines %d\n",lines);
	
	int temp_lines = lines;		
	int num_of_paths = 0 ;		//before sent msg to child, holds number of paths to sent
	fseek(fp, SEEK_SET, 0);
	if (W>lines)
		W = lines;
	int dirs_per_worker = lines/W;
	int modulo = lines%W;
	if (modulo != 0)
		dirs_per_worker++;
	int tmp_mod = modulo;
	printf("dirs_per_worker %d and workers %d\n",dirs_per_worker,W);
	array = malloc(sizeof(char)*lines);
	// fclose(fp);

	// static struct sigaction act;
	// act.sa_handler = catchchild;
	// // act.sa_handler = SIG_IGN;
	// sigfillset(&(act.sa_mask));
	// sigaction(SIGCHLD,&act,NULL);

	int *pid_ar = malloc(sizeof(int)*W);
	int n = 0;

	

	// create two named piped for communication between parent-child
	if ((mkfifo(FIFO1,PERMS) < 0))				// read : parent <--> write : child
		perror("PARENT cant create FIFO1\n");
	// if ((mkfifo(FIFO2,PERMS) < 0))				// read : child <--> write : parent
	// 	perror("PARENT cant create FIFO2\n");

	int readfd , writefd , status;
	for (i=0;i<W;i++)
	{
		pid_ar[i] = fork();
		if (!pid_ar[i])
		{
			char *name = malloc(sizeof(char)*(strlen(FIFO)+10));
			sprintf(name, "%s%d", FIFO,getpid());
			
			int num_of_paths;
			if ((mkfifo(name, PERMS)<0))
				perror("Error : creating FIFO\n");
			
			printf("New Worker with pid %d and fifo %s\n",getpid(),name);
			while ((readfd = open(name, O_RDONLY|O_NONBLOCK))<0);
			while ((n=read(readfd, &num_of_paths, sizeof(int)))<=0); 		// read number of paths 
			printf("num_of_paths %d\n", num_of_paths);
			char **path_array = malloc(sizeof(char*)*num_of_paths);
			char *tmp_buff; 
			tmp_buff = malloc(sizeof(char)*(max_chars));
			for (y=0;y<num_of_paths;y++)
			{
				while ((n=read(readfd,tmp_buff, sizeof(char)*(max_chars)))<=0);
				path_array[y] = malloc(sizeof(char)*(strlen(tmp_buff)+1));
				strncpy(path_array[y], tmp_buff , strlen(tmp_buff));
			}
			close(readfd);
			free(tmp_buff);
			
			for (y=0;y<num_of_paths;y++)
			{
				printf("CHILD %d -> %s\n", getpid(),path_array[y]);
				free(path_array[y]);
			}
			free(path_array);
			free(pid_ar);
			free(name);
			exit(0);
		}
		else
		{	
			char *name = malloc(sizeof(char)*(strlen(FIFO)+10));
			sprintf(name, "%s%d", FIFO,pid_ar[i]);
			if (modulo)									//split work equally
			{
				if (temp_lines>=dirs_per_worker && tmp_mod>0)
				{
					num_of_paths = dirs_per_worker;
					tmp_mod--;
				}
				else 
					num_of_paths = dirs_per_worker-1;
			}
			else
				num_of_paths = dirs_per_worker;
			while ((writefd = open(name,O_WRONLY|O_NONBLOCK))<0);
			write(writefd, &num_of_paths, sizeof(int));		//inform child how many paths to wait
			for (y=0;y<num_of_paths;y++)
			{
				getline(&buff,&buff_size,fp);
				if (buff[strlen(buff)-1] == '\n')
				{
					printf("------EDWWW\n");
					buff[strlen(buff)-1] = '\0';
				}
				else
				{
					printf("--------HEREEEEEEE\n");
					buff[max_chars-1] = '\0';
				}

				printf("PARENT->%s\n",buff);
				write(writefd, buff, sizeof(char)*max_chars); //enonei to teleutaio string , lathos
				temp_lines--;
				free(buff);
				buff = NULL;
				buff_size = 0;
			}
			close(writefd);
			printf("Just finish sending paths\n");
			// free(buff);
			unlink(name);
			free(name);
			num_of_paths = 0;
		}
	}

	pid_t pid;
	for (i=0;i<W;i++)
	{
		pid = waitpid((pid_t)(-1),&status, 0) ;
		printf("Waited for %d\n",pid);
	}
	unlink(FIFO1);
	// unlink(FIFO2);
	fclose(fp);
	free(pid_ar);
	printf("~End of parent!~\n");
	return 0;
}
