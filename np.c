#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>


#define FIFO1 "/home/thanos/Desktop/fifo1"
#define FIFO2 "/home/thanos/Desktop/fifo2"
#define PERMS 0666

void catchinterrupt(int signo)
{
	printf("Catching: signo=%d\n",signo);
	printf("Catching: returning\n");
}

int main(int argc , char *argv[])
{
	static struct sigaction act;
	// act.sa_handler = catchinterrupt;
	act.sa_handler = SIG_IGN;
	sigfillset(&(act.sa_mask));
	sigaction(SIGINT,&act,NULL);	//ctrl-C
	sigaction(SIGTSTP,&act,NULL);	//ctrl-Z

	printf("sleep call #1\n");
	sleep(1);
	printf("sleep call #2\n");
	sleep(1);
	printf("sleep call #3\n");
	sleep(1);
	printf("sleep call #4\n");
	sleep(1);

	act.sa_handler = SIG_DFL;		//reestablich default behavior
	sigaction(SIGINT,&act,NULL);	//default for ctrl-C

	printf("sleep call #1\n");
	sleep(1);
	printf("sleep call #2\n");
	sleep(1);
	printf("sleep call #3\n");
	sleep(1);
	printf("sleep call #4\n");
	sleep(1);
	printf("Exiting\n");
	exit(0);
}