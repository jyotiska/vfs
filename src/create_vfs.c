#include<stdio.h>
#include "../include/vfs_main.h"
#include<string.h>
#include<malloc.h>

#define per_fd 0.1
int create_vfs(char* vfs_name, long vfs_size)
{
	FILE* stream;

	int count = 0;
   	int numwritten;
	int error = 0;

	long no_of_blocks;
	long no_of_file_descriptor;
	long i;
        long no_of_file_blocks;
        long space_left;
        long f_size;
	 
        struct main_header* mh;
	struct file_descriptor *fd;
        struct block *b;
       
        int *p;

	stream = fopen(vfs_name, "r+b");
	if(stream!=NULL)
	{
		error = 1;
		fclose(stream);
		return error;
	}

	if(strlen(vfs_name)>30)
	{
		error = 5;
		return error;
	}

	char *name =(char *)malloc(sizeof(char)*100);
	strcpy(name,vfs_name);
  	 for(i=0; i<strlen(name); i++)
	{
		if(name[i]==47)
		{
			error = 3;
			return error;
		}
	}
        i=0;

	if(vfs_size<1000 || vfs_size>1024000)
	{
		error = 4;
		return error;
	}


        p=malloc(sizeof(int*));
        mh=malloc(sizeof(struct main_header));
        fd=malloc(sizeof(struct file_descriptor));
	b=malloc(sizeof(struct block));
	
	no_of_file_descriptor=(long)((vfs_size)/sizeof(struct block));
    	strcpy(mh->file_system_label,vfs_name);
	mh->max_file_descriptor=no_of_file_descriptor;
	mh->used_file_descriptor=0;
	
	stream = fopen(vfs_name, "w+b");
	if(stream==NULL)
	{
		error = 2;
		return error;
	}
	
	fwrite(mh, sizeof(struct main_header), 1, stream);

	fclose(stream);
	stream = fopen(vfs_name, "r+b");
	size_t result = fread(mh,sizeof(struct main_header),1,stream);
	fclose(stream);
	
      	stream = fopen(vfs_name, "r+b");
	fseek (stream,sizeof(struct main_header),SEEK_SET);

	 for(i=0; i<no_of_file_descriptor; i++)
	{
		fwrite(fd, sizeof(struct file_descriptor), 1, stream);
	}

	int array[no_of_file_descriptor];
	for(i=0;i<no_of_file_descriptor;i++)
	{
		array[i] = 0;
	}

	fwrite(array, sizeof(array), 1, stream);

	no_of_file_blocks = (long)((vfs_size)/sizeof(struct block));
	
	for(i=0; i<no_of_file_blocks; i++)
	{
		fwrite(b, sizeof(struct block), 1, stream);
	}
    
        fseek(stream,0L,SEEK_END);
        f_size=ftell(stream);
	fclose(stream);
	long totalfilesize = vfs_size+sizeof(struct file_descriptor)*no_of_file_descriptor+sizeof(struct main_header)+sizeof(array);
     	
	return error;
}
