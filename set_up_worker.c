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
	size_t buff_size;

	dp = opendir(path_array);
	if (dp == NULL)
		fprintf(stderr, "ERROR in open directory\n");
	printf("%s:\n",path_array);
	while ((entry = readdir(dp)) != NULL)
	{

		if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
			continue;
		char *filename = malloc(sizeof(char)*(strlen(path_array)+strlen(entry->d_name)+2));
		sprintf(filename, "%s/%s",path_array,entry->d_name);
		
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
		insert(info,filename,count_lines,max,offset_array,pos);	
		map_file(file,info,filename);				//function to map file
		fclose(file);
		free(filename);
	}
	closedir(dp);
}