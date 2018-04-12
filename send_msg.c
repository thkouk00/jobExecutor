#include "send_msg.h"

void send_msg(FILE **fp,char *name,int max_chars ,int pid , int *modulo , int *temp_lines , int *dirs_per_worker , int *tmp_mod)
{
	int writefd , num_of_paths;
	int y;
	char *buff = NULL;
	size_t buff_size =0;
	sprintf(name, "%s%d", FIFO,pid);
	if (*modulo)									//split work equally
	{
		if (*temp_lines>=*dirs_per_worker && *tmp_mod>0)
		{
			num_of_paths = *dirs_per_worker;
			*tmp_mod-=1;
		}
		else 
			num_of_paths = *dirs_per_worker-1;
	}
	else
		num_of_paths = *dirs_per_worker;
	while ((writefd = open(name,O_WRONLY|O_NONBLOCK))<0);
	write(writefd, &num_of_paths, sizeof(int));		//inform child how many paths to wait
	for (y=0;y<num_of_paths;y++)
	{
		getline(&buff,&buff_size,*fp);
		buff[strlen(buff)-1] = '\0';

		// printf("PARENT->%s\n",buff);
		write(writefd, buff, sizeof(char)*max_chars); 
		*temp_lines-=1;
		free(buff);
		buff = NULL;
		buff_size = 0;
	}
	close(writefd);
	// printf("Just finish sending paths temp_lines %d\n",*temp_lines);
	// unlink(name);
	// free(name);
}