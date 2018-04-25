#include "set_up_worker.h"

void set_up_worker(listNode **info,char *path_array)
{
	DIR *dp;
	struct dirent *entry;
	FILE *file;
	int x, max = 0;
	int count_lines = 0;
	int *offset_array;
	char *buff=NULL;
	int *realloc_buff=NULL;
	size_t buff_size;

	dp = opendir(path_array);
	if (dp == NULL)
		fprintf(stderr, "ERROR in open directory\n");
	while ((entry = readdir(dp)) != NULL)		//take every text file from directory
	{

		if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
			continue;
		char *filename = malloc(sizeof(char)*(strlen(path_array)+strlen(entry->d_name)+2));
		sprintf(filename, "%s/%s",path_array,entry->d_name);
		file = fopen(filename, "r");
		if (file == NULL)
			perror("Failed to open file :: set_up_worker.c");
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
				realloc_buff = realloc(offset_array, sizeof(int)*(tmp_size));
				if (realloc_buff != NULL)
					offset_array = realloc_buff;
				else
					printf("REALLOC FAILED\n");
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
		free(buff);				//extra
		buff = NULL;
		fseek(file, 0, SEEK_SET);
		pos--;	 
		insert(info,filename,count_lines,max,offset_array,pos);	
		map_file(file,info,filename);				//function to map file
		fclose(file);
		free(filename);
		free(offset_array);		//extra
	}
	closedir(dp);
}