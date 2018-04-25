#include "send_msg.h"

void send_msg(FILE **fp,char *name,char *name2,int max_chars ,int pid ,int *paths_to_pid, int *modulo , int *temp_lines , int *dirs_per_worker , int *tmp_mod,int lines)
{
	int writefd , num_of_paths;
	int y;
	char *buff = NULL;
	size_t buff_size =0;
	sprintf(name, "%s%d", FIFO,pid);
	sprintf(name2, "%s_2",name);
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
	while ((writefd = open(name,O_WRONLY|O_NONBLOCK))<0)
		usleep(2000);
	write(writefd, &num_of_paths, sizeof(int));		//inform child how many paths to wait
	for (y=0;y<num_of_paths;y++)
	{
		getline(&buff,&buff_size,*fp);
		buff[strlen(buff)-1] = '\0';

		write(writefd, buff, sizeof(char)*max_chars); 
		*temp_lines-=1;
		free(buff);
		buff = NULL;
		buff_size = 0;
		for (int k=0;k<lines;k++)
		{
			if (paths_to_pid[k] == -1)
			{
				paths_to_pid[k] = pid;
				break;
			}
		}
	}
	close(writefd);
}