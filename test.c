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

#define FIFO1 "/home/thanos/Desktop/fifo1"
#define FIFO2 "/home/thanos/Desktop/fifo2"
#define PERMS 0666


void catchchild(int signo)
{
	pid_t pid ;
	int status;
	// while ((pid = waitpid((pid_t)(-1),&status, WNOHANG)) > 0);
	// while ((pid = wait(&status))>0);
	printf("Catching: signo=%d and pid %d\n",signo,pid);
	printf("Catching: returning\n");
}

int main(int argc , char* argv[])
{
	int W = 3;
	static struct sigaction act;
	act.sa_handler = catchchild;
	// act.sa_handler = SIG_IGN;
	sigfillset(&(act.sa_mask));
	sigaction(SIGCHLD,&act,NULL);

	int *pid_ar = malloc(sizeof(int)*W);
	int size_buf = 50;
	char temp[size_buf];
	int n = 0;

	int readfd , writefd , status;
	int i;
	int num1,num2;
	char temp_s[20]="12|24";
	sprintf(num1,num2, "%d|%d",temp_s);
	printf("%d kai %d\n", num1,num2);
	for (i=0;i<W;i++)
	{
		pid_ar[i] = fork();
		if (!pid_ar[i])
		{
			printf("New Worker with pid %d\n",getpid());
			while ((readfd = open(FIFO1,O_RDONLY|O_NONBLOCK))<0);
			dup2(readfd, 0);
			size_t bufsize;
			char *buffer=NULL;
			while ((getline(&buffer,&bufsize,stdin))<=0);
			printf("Msg: %s , pid %d\n", buffer,getpid());
			free(buffer);
			close(readfd);
			free(pid_ar);
			printf("Exiting %d\n",getpid());
			exit(0);
		}
		else
		{
			if ((mkfifo(FIFO1,PERMS) < 0))
				perror("PARENT cant create FIFO\n");	
			while ((writefd = open(FIFO1,O_WRONLY|O_NONBLOCK))<0);
			write(writefd, "WELCOME TO SYSPRO", size_buf);
			printf("Just sent the msg to %d\n",pid_ar[i]);
			close(writefd);
			unlink(FIFO1);
			// sleep(1);

		}
	}
	pid_t pid;
	for (i=0;i<W;i++)
	{
		pid = waitpid((pid_t)(-1),&status, 0) ;
		printf("Waited for %d\n",pid);
	}
	// while ((wait(&status))>0);
	// waitpid(-1, &status,0);
	// wait(NULL);				//perimenw mono to teleytaio na teliosei , ta alla exount teliosei
	// close(writefd);
	// unlink(FIFO1);
	free(pid_ar);
	printf("~End of parent!~\n");
	return 0;
}
