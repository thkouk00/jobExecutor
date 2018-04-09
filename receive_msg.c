void receive_msg()
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
	char *tmp_buff = malloc(sizeof(char)*(max_chars));
	for (y=0;y<num_of_paths;y++)
	{
		while ((n=read(readfd,tmp_buff, sizeof(char)*(max_chars)))<=0);
		path_array[y] = malloc(sizeof(char)*(strlen(tmp_buff)+1));
		strncpy(path_array[y], tmp_buff , strlen(tmp_buff));
	}
	close(readfd);
	free(tmp_buff);
}