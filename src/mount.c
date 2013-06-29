#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include "../include/vfs_main.h"
#include "../include/nary.h"
#include "../include/hash.h"



int mount(char *vfs_name,struct ntree* root,struct hash_node block[])
{
        FILE * pFile;
	int* buffer;
	int* occupied_fd;
	size_t result;
	int flag=1;
	int i=0,j=0;
	int error = 0;

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
	 
	pFile = fopen(vfs_name,"r+b");
	if (pFile==NULL) 
	{
		error = 1;
		return error;
	}
	 
	fseek (pFile,0,SEEK_SET);
	long size;
	struct main_header *mho=(struct main_header*)malloc(sizeof(struct main_header));
	result=fread(mho,sizeof(struct main_header),1,pFile);
	fclose(pFile);
	size=mho->max_file_descriptor*sizeof(struct block);
	
	int loopcount = size/sizeof(struct block);
	long lSize = size/sizeof(struct block) * sizeof(int);
	long offset = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);
	
	pFile = fopen(vfs_name,"r+b");
	fseek (pFile,offset,SEEK_SET);
	

	buffer = (int*)malloc(sizeof(int)*lSize);
	result = fread(buffer,sizeof(int),lSize,pFile);

	if (buffer == NULL) 
	{
		error =2;
		return error;
	}

	int *primary_freelist=(int *)buffer;

	occupied_fd=(int*) malloc (sizeof(int)*lSize);
	
	for(j=0; j<loopcount; j++)
	{
		occupied_fd[j]=-1;
	}

	j=0;
	for(i=0;i<loopcount;i++)
	{
		if(primary_freelist[i]==1)
		{
			occupied_fd[j++]=i;
		}
	}

	char* sbuffer;
	
	initialize_hashtable(block);

	for(i=0; i<j; i++)
	{
		fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		struct file_descriptor* s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);

		root=nary_insert(s,root);
		insert_hashtable(block,s);	
	}

	fclose (pFile);	
        return error;
}
