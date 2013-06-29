#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include "../include/vfs_main.h"
#include "../include/nary.h"
#include "../include/hash.h"
#include "../include/create_vfs.h"
#include "../include/mount.h"

char* returndirname(char *fullpath)
{
	char *path = (char *)malloc(sizeof(char)*100);
	int i,j,pos;
	
	for(i=0;i<strlen(fullpath);i++)
	{
		if(fullpath[i] == '/')
			pos = i;
	}

	for(i=0;i <= pos;i++)
	{
		path[i] = fullpath[i];
	}
	path[i]='\0';

	if(pos == 0)
	{
		path="invalid";
	}

	return path;
}

char* returnfilename(char *fullpath)
{
	char *filename = (char *)malloc(sizeof(char)*100);
	int i,j,pos;
	
	for(i=0;i<strlen(fullpath);i++)
	{
		if(fullpath[i] == '/')
			pos = i;
	}

	for(j=0,i=pos+1;i < strlen(fullpath);i++,j++)
	{
		filename[j] = fullpath[i];
	}
	filename[j]='\0';

	return filename;
}

char* returnonlydirname(char *fullpath)
{
	char *path = (char *)malloc(sizeof(char)*100);
	int i,j,pos;
	
	for(i=0;i<strlen(fullpath);i++)
	{
		if(fullpath[i] == '/')
			pos = i;
	}

	for(i=0;i<=pos-1;i++)
	{
		path[i] = fullpath[i];
	}
	path[i]='\0';
	return path;
}

int checkpath(char *fullpath,struct hash_node block[])
{
	if(strcmp(fullpath,"bond/")==0)
	{
		return 0; // bond/ will always exist, so return 0
	}
	else
	{
		char* path = returndirname(fullpath);
		char* filename = returnfilename(fullpath);
		int rvalue = search_hashtable_path(block,filename,path);

		if(rvalue == 999)
		{
			return 1; //Directory path does not exist
		}
		else
		{
			return 0; //Directory path exists
		}
	}
}

int My_addfile(char* path,char* filename,char* sourcefile,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	FILE *sfile; //Source File Pointer
	FILE *vfile; //VFS File Pointer

	int i,j,k;
	int error = 0;
	int fillcheck = 0;

	int* buffer; //To store the primary freelist
	
	for(k = 0;k < strlen(filename);k++){
		if(filename[k] == 47){
			error = 2; //Error - Invalid character in filename
			return error;
		}
	}

	long rvalue = search_hashtable_path(block,filename,path); //Find the index of the file
	if(rvalue != 999)
	{
		error = 3; //Error - File already exists
		return error;
	}

	char data[sizeof(struct block)]; //To store the file content to be added
	long lsize = size/sizeof(struct block); //Number of FD

	struct file_descriptor *fd = (struct file_descriptor*)malloc(sizeof(struct file_descriptor));

	long offset = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor); //Location of Primary Freelist
	
	sfile = fopen(sourcefile, "r");
	if(sfile == NULL)
	{
		error = 8;
		return error;
	}
	fseek(sfile,0,SEEK_END);
	size_t file_size = ftell(sfile);
	rewind(sfile);
	
	if(file_size > sizeof(struct block))
	{
		error = 6; //Error - File too large
		return error;
	}

	data[0]=fgetc(sfile); //Write the contents of source file into data
	i=0;
	while(data[i]!=EOF)
	{
		data[++i]=fgetc(sfile);
	}
	data[i]='\0';
	fclose(sfile);
	
	vfile = fopen(vfs_name,"r+b");
	if(vfile == NULL)
	{
		error = 5; //Error - Cannot write to data file
		return error;
	}
	fseek(vfile,offset,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*lsize);
	fread(buffer,1,lsize*sizeof(int),vfile);
	int *primary_freelist=(int *)buffer; //Freelist is copied into Primary Memory
	
	for(j=0;j<lsize;j++){
		if(primary_freelist[j] == 0){
			strcpy(fd->file_name,filename);
			strcpy(fd->location_full_path,path);
			fd->file_type = 1;
			fd->file_size = 1000;
			fd->location_block_number = j;
			primary_freelist[j] = 1;
			insert_hashtable(block,fd);
			root = nary_insert(fd,root);
			fillcheck = 1;
			break;
		}
	}

	if(fillcheck == 0)
	{
		error = 4; //Error - File system full
		return error;
	}

	fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(j),SEEK_SET);
	fwrite(fd,sizeof(struct file_descriptor),1,vfile);

	fseek(vfile,offset+lsize*sizeof(int)+(j)*sizeof(struct block),SEEK_SET);
	fwrite(data,sizeof(struct block),1,vfile);

	fseek(vfile,offset,SEEK_SET);
	fwrite(primary_freelist,sizeof(int),lsize,vfile);

	fclose(vfile);
	return error;
}

void copy_vfs(char* sourcefile,char* filename,int size)
{
	FILE * sour_F;
	FILE * dest_F;
	int i,j;
	int flag=1,flag1,flag2;
	int* buffer;
	size_t result;
	size_t file_size;
	char data[size];

	sour_F = fopen(sourcefile, "r+b");
	fread(data,size,1,sour_F);	
	fclose(sour_F);
	dest_F = fopen(filename,"w+b");	
	fwrite(data,size,1,dest_F);			
	fclose(dest_F);
}
	
int My_listFile(char* dirpath,char* outfilename,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	int error = 0;	
	char *check = (char *)malloc(sizeof(char)*5);
	check = ".txt";
	int i,j,k;
	char name1[30];
	char* path = returndirname(dirpath); //Returns the path name 
	char* filename = returnfilename(dirpath); //Returns the filename
	i = strlen(filename) - 4;
	k = 0;
	for(j = i;j < strlen(filename);j++){
		name1[k] = filename[j];
		k++;
	}
	name1[k] = '\0';
	/*if(!strstr(filename,check))
	{
		error = 2; //ERROR - Not a text file
		return error;
	}*/
	
		
		if(!(name1[0] == '.' && name1[1] == 't' && name1[2] == 'x' && name1[3] == 't'))
		{
			error = 2; //ERROR - Not a text file
			return error;
		}
		
		FILE *vfile;
		FILE *dfile;
		char* buffer;
		long lsize = size/sizeof(struct block);
		long tot_size = sizeof(struct main_header) + (lsize * sizeof(struct file_descriptor));
		long rvalue = search_hashtable_path(block,filename,path); //Find the index of the file
		if(rvalue == 999)
		{
			error = 1; //Error - Source file not found
			return error;
		}

		long offset = tot_size + (lsize*sizeof(int)) + (rvalue)*sizeof(struct block);

		vfile = fopen(vfs_name, "r+b");
		fseek(vfile,offset,SEEK_SET);
		buffer = (char*) malloc (sizeof(struct block));
		fread(buffer,1,sizeof(struct block),vfile); //Read the content of that file block in the buffer
		dfile = fopen(outfilename, "w");
		if(dfile == NULL)
		{
			error = 3; //Error - Cannot create output file
			return error;
		}
		fprintf(dfile,"%s",buffer); //Write the content of the file into the destination file

		fclose(vfile);
		fclose(dfile);
	
		return error;
	
}

int My_updateFile(char* dirpath,char* destfile,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	int error = 0;
	char* path = returndirname(dirpath);
	char* filename = returnfilename(dirpath);
	
	FILE *vfile;
	FILE *dfile;
	char* buffer;
	char* content = (char *)malloc(sizeof(char)*100);
	long lsize = size/sizeof(struct block);
	long tot_size = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);
	long rvalue = search_hashtable_path(block,filename,path); //Returns the index of the file
	if(rvalue == 999)
	{
		error = 1; //Error - Internal File not found
		return error;
	}
	long offset = tot_size+lsize*sizeof(int)+(rvalue)*sizeof(struct block);

	dfile=fopen(destfile,"r");
	if(dfile == NULL)
	{
		error = 2; //Error - External File not found
		return error;
	}
	fseek(dfile,0,SEEK_END);
	size_t file_size = ftell(dfile);
	rewind(dfile);
	if(file_size > sizeof(struct block))
	{
		error = 3; //Error - External file too large
		return error;
	}
	fread(content,sizeof(struct block),1,dfile); //Read the contents of the source file in content

	vfile = fopen(vfs_name, "r+b");
	fseek(vfile,offset,SEEK_SET);
	fwrite(content,sizeof(struct block),1,vfile); //Write the content into the destination file(VFS)

	fclose(dfile);
	fclose(vfile);

	return error;
}

int My_copyFile(char* dirpath,char* destpath,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	int error = 0;
	char* sourcepath = returndirname(dirpath);
	char* filename = returnfilename(dirpath);

	FILE *vfile;
	char* buff;
	int* buffer;
	int j,k;
	int fillcheck = 0; //To check whether the file system is full or not
	char data[sizeof(struct block)]; //Take a block of size of a file block
	long lsize = size/sizeof(struct block); //Number of FD
	long tot_size = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);

	if(strcmp(destpath,"bond/")==1)
	{
		char* semipath = returnonlydirname(destpath);
		char* destdir = returndirname(semipath);
		char* destfile = returnfilename(semipath);
		int dvalue = search_hashtable_path(block,destfile,destdir);
		if(dvalue == 999)
		{
			error = 2; //Error - Cannot find destination path
			return error;
		}
	}

	long rvalue = search_hashtable_path(block,filename,sourcepath);
	if(rvalue == 999)
	{
		error = 1; //Error - Cannot find source file
		return error;
	}
	if(rvalue == 9999)
	{
		error = 3; //Error - Cannot copy directory to file
		return error;
	}
	long offset = tot_size+lsize*sizeof(int)+(rvalue)*sizeof(struct block);
	struct file_descriptor *fd = (struct file_descriptor*)malloc(sizeof(struct file_descriptor));

	vfile = fopen(vfs_name, "r+b");
	fseek(vfile,tot_size,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*lsize);
	fread(buffer,1,lsize*sizeof(int),vfile);

	int *primary_freelist=(int *)buffer;

	for(j = 0;j<lsize;j++){
		if(primary_freelist[j] == 0){
			strcpy(fd->file_name,filename);
			strcpy(fd->location_full_path,destpath);
			fd->file_type = 1;
			fd->file_size = 1000;
			fd->location_block_number = j;
			primary_freelist[j] = 1;
			insert_hashtable(block,fd);
			root=nary_insert(fd,root);
			fillcheck=1; //In case of a free block, change the value to 1
			break;
		}
	}

	if(fillcheck == 0)
	{
		error = 4; //Error - File system full
		return error;
	}
	
	fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(j),SEEK_SET);
	fwrite(fd,sizeof(struct file_descriptor),1,vfile); //Write the file descriptor

	fseek(vfile,tot_size+lsize*sizeof(int)+(j)*sizeof(struct block),SEEK_SET);
	fwrite(data,sizeof(struct block),1,vfile); //Write the data

	fseek(vfile,tot_size,SEEK_SET);
	fwrite(primary_freelist,sizeof(int),lsize, vfile); //Write the freelist

	fclose(vfile);

	return error;
}

int My_removeFile(char* dirpath,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{	
	int error = 0;
	char* path = returndirname(dirpath);
	char* filename = returnfilename(dirpath);
	
	FILE *vfile;
	int *buffer;
	int i,k;
	char data[sizeof(struct block)];
	for(i=0;i<sizeof(struct block);i++) { //Fill a block with blank spaces
		data[i] = ' ';
	}

	long lsize = size/sizeof(struct block);
	long tot_size = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);
	long rvalue = search_hashtable_path(block,filename,path);
	if(rvalue == 999)
	{
		error = 1; //Error - Cannot find Specified file;
		return error;
	}
	long offset = tot_size+lsize*sizeof(int)+(rvalue)*sizeof(struct block);
	struct file_descriptor *fd = (struct file_descriptor*)malloc(sizeof(struct file_descriptor)); //Blank FD to write
	struct file_descriptor *fdnew = (struct file_descriptor*)malloc(sizeof(struct file_descriptor)); //FD to store the current contents
	
	vfile = fopen(vfs_name, "r+b");
	fseek(vfile,tot_size,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*lsize);
	fread(buffer,1,lsize*sizeof(int),vfile); //Read the freelist

	fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(rvalue),SEEK_SET);
	fread(fdnew,sizeof(struct file_descriptor),1,vfile); //Read the FD and store the current contents into 'fdnew'

	int *primary_freelist=(int *)buffer; //Copy freelist into primary memory
	primary_freelist[rvalue] = 0; //Set the value of freelist index to 0
	
	fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(rvalue),SEEK_SET);
	fwrite(fd,sizeof(struct file_descriptor),1,vfile); //Write a blank fd

	fseek(vfile,tot_size+lsize*sizeof(int)+(rvalue)*sizeof(struct block),SEEK_SET);
	fwrite(data,sizeof(struct block),1,vfile); //Write a blank data block

	fseek(vfile,tot_size,SEEK_SET);
	fwrite(primary_freelist,sizeof(int),lsize, vfile); //Write the freelist
	
	nary_delete(fdnew,root);
	delete_hashtable(block,fdnew);

	fclose(vfile);

	return error;
}

int My_moveFile(char* dirpath,char* destpath,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	int error = 0;
	char* path = returndirname(dirpath);
	char* filename = returnfilename(dirpath);
	
	FILE *vfile;
	char* buffer;
	long lsize = size/sizeof(struct block);
	long tot_size = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);

	if(strcmp(destpath,"bond/")==1)
	{
		char* semipath = returnonlydirname(destpath);
		char* destdir = returndirname(semipath);
		char* destfile = returnfilename(semipath);
		semipath = strcat(semipath,"/");
		int dvalue = search_hashtable_path(block,filename,semipath);
		if(dvalue == 999)
		{
			error = 2; //Error - Cannot find destination path
			return error;
		}
	}

	long rvalue = search_hashtable_path(block,filename,path);
	if(rvalue == 999)
	{
		error = 1; //Error - Cannot find source file
		return error;
	}
	struct file_descriptor *fd = (struct file_descriptor*)malloc(sizeof(struct file_descriptor));

	vfile = fopen(vfs_name, "r+b");
	fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(rvalue),SEEK_SET);
	fread(fd,sizeof(struct file_descriptor),1,vfile); //Read the FD

	nary_delete(fd,root);
	delete_hashtable(block,fd);

	strcpy(fd->file_name,filename);
	strcpy(fd->location_full_path,destpath);
	fd->file_type = 1;
	fd->file_size = 1000;
	fd->location_block_number = rvalue;

	fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(rvalue),SEEK_SET);
	fwrite(fd,sizeof(struct file_descriptor),1,vfile); //Write the modified file info into VFS

	nary_insert(fd,root);
	insert_hashtable(block,fd);

	fclose(vfile);

	return error;
}

int My_searchFile(char* prefix,char* filename,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	FILE *vfile;
	FILE *dfile;
	int i,j;
	int* buffer;
	char *pch;
	int slashcheck=0;
	int plen = strlen(prefix); //Size of the prefix

	for(i=0;i<plen;i++)
	{
		if(prefix[i] == '/')
		{
			slashcheck++;
		}
	}

	if(slashcheck == 0)
	{
		return 2; //Error because no slash/directory(not a full path)
	}

	if(slashcheck != 0)
	{
		char* path = returndirname(filename);
		char* file = returnfilename(filename);
		long rvalue = search_hashtable_path(block,file,path); //Find the index of the file

		if(rvalue != 999)
		{
		long lsize = size/sizeof(struct block);
		struct file_descriptor *fd = (struct file_descriptor*)malloc(sizeof(struct file_descriptor));
		long tot_size = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);
		long offset = sizeof(struct main_header) + size/sizeof(struct block) * sizeof(struct file_descriptor);
	
		vfile = fopen(vfs_name,"r+b");

		fseek(vfile,offset,SEEK_SET);
		buffer = (int*)malloc(sizeof(int)*lsize);
		fread(buffer,1,lsize*sizeof(int),vfile);

		int *primary_freelist=(int *)buffer;
	
		dfile = fopen(filename,"w");

		for(j = 0; j<size/sizeof(struct block);j++){ //Loop continues for number of FDs
			if(primary_freelist[j] == 1){ //If block is occupied
				fseek(vfile,sizeof(struct main_header)+sizeof(struct file_descriptor)*(j),SEEK_SET);
				fread(fd,sizeof(struct file_descriptor),1,vfile); //Read the FD

				if(fd->file_type == 1) //If this is a file not a directory
				{
					pch=strstr(fd->file_name,prefix); //Check for the match
					if(pch)
					{
						if(!strcmp(pch,fd->file_name))
						{
							fprintf(dfile,"%s\n",fd->file_name); //Write into local file if match is found
						}
					}
				}
			}
		}

	fclose(vfile);
	fclose(dfile);
	}
	}

	
	return 0;
}

int My_exportFile(char* dirpath,char* outfilename,int size,char* vfs_name,struct ntree *root,struct hash_node block[])
{
	int error = 0;
	char* path = returndirname(dirpath);
	char* filename = returnfilename(dirpath);
	
	FILE *vfile;
	FILE *dfile;
	char* buffer;
	long lsize = size/sizeof(struct block);
	long tot_size = sizeof(struct main_header) + (lsize * sizeof(struct file_descriptor));
	long rvalue = search_hashtable_path(block,filename,path); //Find the index of the file
	if(rvalue == 999)
	{
		error = 1; //Error - Cannot find source file
		return error;
	}
	
	long offset = tot_size + (lsize*sizeof(int)) + (rvalue)*sizeof(struct block);

	vfile = fopen(vfs_name, "r+b");
	fseek(vfile,offset,SEEK_SET);
	buffer = (char*) malloc (sizeof(struct block));
	fread(buffer,1,sizeof(struct block),vfile); //Read the content of that file block in the buffer
	dfile = fopen(outfilename, "w");
	if(dfile == NULL)
	{
		error = 2; //Error - Cannot create output file
		return error;
	}
	fprintf(dfile,"%s",buffer); //Write the content of the file into the destination file

	fclose(vfile);
	fclose(dfile);
	return error;
	
}
