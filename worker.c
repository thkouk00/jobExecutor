#include "worker.h"

static volatile sig_atomic_t triggered = 0;

void catchsignal(int signo)
{
	if (signo == SIGCLD)
	{
		// while (waitpid(-1,NULL,WNOHANG)>0);
	}
	if (signo == SIGALRM)
		write(1, "Alarm finished", sizeof(char)*strlen("Alarm finished"));
	if (signo == SIGTERM)
	{
		triggered = 1;
		write(1, "ME SKOTOSAN\n", sizeof(char)*strlen("ME SKOTOSAN\n"));
	}
}

void worker(int max_chars,FILE *fp,int *pid_ar)
{
	static struct sigaction act;
	act.sa_handler = catchsignal;
	sigfillset(&(act.sa_mask));
	sigaction(SIGALRM,&act,NULL);
	// sigaction(SIGCHLD,&act,NULL);
	sigaction(SIGUSR1,&act,NULL);
	sigaction(SIGTERM,&act,NULL);

	time_t curtime;
	char *time_buff;		//take time for log file
	char *log_name = malloc(sizeof(char)*(strlen(LOG)+15));
	sprintf(log_name, "%s%d.txt",LOG,getpid());
	// printf("LOG NAME %s\n", log_name);
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

	int readfd , writefd , n;
	int num_of_paths , y;
	if ((mkfifo(name, PERMS)<0))							// create pipes
		perror("Error : creating FIFO\n");
	if ((mkfifo(name2, PERMS)<0))
		perror("Error : creating FIFO\n");

	while ((readfd = open(name, O_RDONLY|O_NONBLOCK))<0)
		usleep(2000);
	while ((n=read(readfd, &num_of_paths, sizeof(int)))<=0) 		// read number of paths 
		usleep(2000);
	// printf("NUMOFPAATHS %d %d\n",num_of_paths,getpid());

	//na to elegxw ti pairnei gia na mi skasei kapoia stigmi , giati to bazw kateuthian se 
	// metabliti (gia to read apo panw)

	char **path_array = malloc(sizeof(char*)*num_of_paths);
	char *tmp_buff = malloc(sizeof(char)*(max_chars));
	for (y=0;y<num_of_paths;y++)
	{
		while ((n=read(readfd,tmp_buff, sizeof(char)*(max_chars)))<=0)
			usleep(2000);
		path_array[y] = malloc(sizeof(char)*(strlen(tmp_buff)+1));	//htan +1
		strncpy(path_array[y], tmp_buff , strlen(tmp_buff));
		path_array[y][strlen(tmp_buff)] = '\0';
	}
	// close(readfd);
	free(tmp_buff);
	// one trie per Worker , one map per file
	//list to hold maps for every doc
	listNode **info = malloc(sizeof(listNode*)*num_of_paths);	
	trieNode_t *trie;
	CreateTrie(&trie);

	// printf("**PATHS for %d::\n",getpid());
	// for (y=0;y<num_of_paths;y++)
	// 	printf("%s.\n", path_array[y]);

	int *offset_array ;
	for (y=0;y<num_of_paths;y++)
	{
		info[y] = NULL;
		set_up_worker(&info[y],path_array[y]);			//takes docfiles and map them
		fill_trie(&info[y], &trie);						//insert to trie
		// printf("**MEXRI EDW OLA OK %d with loop %d/%d \n",getpid(),y,num_of_paths);

	}
	printNode(&trie, "syspro");
	printNode(&trie, "to");
	int size_to_read;
	char *buff = NULL;
	tmp_buff = malloc(sizeof(char)*20); 	//buffer to hold number sent from parent

	//worker must wait for message from jobExecutor for queries
	// printf("---WORKER---%s\n",name2);
	while ((writefd = open(name2, O_WRONLY|O_NONBLOCK))<0)
		usleep(2000);
	while(1)			// htan !stop
	{
		if (triggered)
			write(writefd, "~", sizeof(char)*2);		//SIGTERM raised
		else
			write(writefd, "^", sizeof(char)*2);

		pause(); 			//wait for SIGUSR1 to wake up and continue execution 

		while ((n=read(readfd,tmp_buff, sizeof(char)*20))<=0)
			usleep(2000);
		size_to_read = atoi(tmp_buff);
		// read query from jobExecutor
		if (size_to_read != -1)
		{
			buff = malloc(sizeof(char)*(size_to_read)+1);	//htan size to read +1
			while ((n=read(readfd,buff, sizeof(char)*size_to_read))<=0)
				usleep(2000);
			buff[size_to_read] = '\0';
			if (!strncmp(buff, "/search ", strlen("/search ")))
			{
				while ((n=read(readfd,tmp_buff, sizeof(char)*20))<=0)
					usleep(2000);
				int deadline = atoi(tmp_buff);
				// printf("RECEIVE %s and deadline %d\n", buff,deadline);
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
				sprintf(buff1, "%d", count+4); 	//+4 -> 3chars for | and one for end
				write(writefd, buff1, sizeof(char)*20);			//send it to parent
				free(buff1);
				buff1 = malloc(sizeof(char)*(count+3)); 
				sprintf(buff1, "%d|%d|%d",total[0],total[1],total[2]);
				printf("%s\n", buff1);
				write(writefd, buff1, sizeof(char)*(count+4)); 
				time(&curtime);
				time_buff = ctime(&curtime);
				time_buff[strlen(time_buff)-1] = '\0';
				fprintf(f, "%s: wc: chars: %d words: %d lines: %d\n", time_buff,total[0],total[1],total[2]);
				free(buff1);
			}


			free(buff);
			buff = NULL;
		}
		else				// -1 detected , end of worker
		{
			close(readfd);
			close(writefd);
			free(tmp_buff);
			free(buff);
			buff = NULL;
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
	free(info);
	free(path_array);
	free(pid_ar);		
	free(name);
	free(name2);
	fclose(fp);				
	FreeTrie(&trie);	
	exit(0);
}