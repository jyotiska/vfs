#include<stdio.h>
#include "../include/vfs_main.h"
#include<string.h>
#include<malloc.h>
#include "../include/unmount.h"

int unmount(char *P1,char *vfs_name)
{
	FILE *pFile;
	int* buffer;
	int error=0;
	size_t result;
	int i=0;

 	pFile = fopen(P1,"r+b");

	char *name =(char *)malloc(sizeof(char)*100);
	strcpy(name,P1);
  	 for(i=0; i<strlen(name); i++)
	{
		if(name[i]==47)
		{
			error = 3;
			return error;
		}
	}
	
	if(pFile==NULL) 
	{
		error = 1;
		return error;
	}

	buffer = (int*)malloc(sizeof(int)*5);
	result = fread(buffer,sizeof(int),5,pFile);

	if (buffer == NULL) 
	{
		error = 2;
		return error;
	}
	fclose(pFile);
	return error;
}
