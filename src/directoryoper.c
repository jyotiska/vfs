#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include "../include/vfs_main.h"
#include "../include/hash.h"
#include "../include/nary.h"
#include "../include/mount.h"
#include "../include/directoryoper.h"

int mymovedir ( char* oldpath, char* newpath, struct ntree *root,struct hash_node block[26],int num_of_blocks,char* vfs_name)
{
	FILE *pFile;
	int *primary_freelist,*buffer;
	char *match;
	int i=0,j=0,flag=1,error=0,d=0,ctr=0;
	int opflag=0;
	int destfound = 0;
	
	char *op = (char *)malloc(sizeof(char)*200);
	char *np = (char *)malloc(sizeof(char)*200);
 	char *pch,*pchdest;
        char root_path[200]="",parent_path[200]="",dest_path[200]="";
	char new[num_of_blocks][200];
	char newdest[num_of_blocks][200];

	int modified_fd[num_of_blocks];
	int mfd_ptr = 0;

	strcpy(op,oldpath);
	strcpy(np,newpath);

	match=strstr(np,op);
	if(match!=NULL)
	{
		error=6;
		return error;
	}

	strcpy(op,oldpath);
	strcpy(np,newpath);


	pchdest = strtok (np,"/");

	while (pchdest != NULL)
  	{
    		strcpy(newdest[d],pchdest);
		d++;
    		pchdest = strtok (NULL, "/");
  	}
	
	char *destdir = (char *)malloc(sizeof(char)*200);
	strcpy(destdir,newdest[d-1]);//source dir
	
	for(ctr=0; ctr<d-1 && d>1; ctr++)
	{
		strcat(dest_path,newdest[ctr]);
	  	strcat(dest_path,"/");
	}


	pch = strtok (op,"/");
	i=0;
	while (pch != NULL)
  	{
    		strcpy(new[i],pch);
		i++;
    		pch = strtok (NULL, "/");
  	}
	
	char *rootdir = (char *)malloc(sizeof(char)*200);
	strcpy(rootdir,new[i-1]);//source dir
        for(j=0;j<i-2&&i>=3;j++)
         {
	   strcat(parent_path,new[j]);
	    strcat(parent_path,"/");
         }
        for(j=0;j<i-1&&i>=2;j++)
         {
	   strcat(root_path,new[j]);
	    strcat(root_path,"/");
         }
	int c=search_hashtable_diroper(block,rootdir,newpath);
        if(c == 0)
          {
          error=5;
          return error;
         }  
	c=search_hashtable_diroper(block,rootdir,root_path);
        if(c == 1)
    	{
          error=1;
          return error;
        }  

	pFile = fopen(vfs_name,"r+b");
	fseek (pFile,sizeof(struct main_header)+sizeof(struct file_descriptor)*num_of_blocks,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*num_of_blocks);
	size_t result = fread(buffer,sizeof(int),num_of_blocks,pFile);
	fclose(pFile);
	
	primary_freelist=(int *)buffer;

	int *occupied_fd=(int*) malloc (sizeof(int)*num_of_blocks);
	for(j=0; j<num_of_blocks; j++)
	{
		occupied_fd[j]=-1;
	}

	j=0;
	for(i=0;i<num_of_blocks;i++)
	{
		if(primary_freelist[i]==1)
		{
			occupied_fd[j++]=i;
		}
	}
	int size_fd = j-1;	

	pFile = fopen(vfs_name,"r+b");
	for(i=size_fd; i>=0; i--)
	{
		fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		struct file_descriptor *s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);
                 
		if(strcmp(newpath,"bond/")==0)
		{
			destfound =1;
			break;
		}

		
		if(strcmp (s->location_full_path,dest_path)==0 && strcmp(s->file_name,destdir)==0 )
		{
			if(s->file_type==1)
			{
				error=7;
				return error;
			}
			destfound =1;
		}
	}
	fclose(pFile);



	if(destfound==0)
	{
		error=2;
		return error;
	}


	pFile = fopen(vfs_name,"r+b");

	for(i=size_fd; i>=0; i--)
	{
		fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		struct file_descriptor *s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);
                 
		char *match = strstr (s->location_full_path,oldpath);
		if(match!=NULL || strcmp(s->file_name,rootdir)==0)
		{
			
			if( s->file_type==1){
                       		error = 4;
				return error;
			}

			char *pch2;
			strcpy(np,newpath);
			char string[200] = "";
			strcpy(string,np);
			char new2[num_of_blocks][200];
			char *read_path=(char *)malloc(sizeof(char)*200);
			strcpy(read_path,s->location_full_path);
			int k=0;
			pch2 = strtok (read_path,"/");
		
			while (pch2 != NULL)
		  	{
		    		strcpy(new2[k],pch2);
				k++;
		    		pch2 = strtok (NULL, "/");
		  	}

			for(j=0; j<k; j++)
			{
				if(strcmp(new2[j],rootdir)==0)
				{
					break;
				}
			}
			while(j<k)
			{
				strcat(string,new2[j]);
				strcat(string,"/");
				j++;
			}
			
			delete_hashtable(block,s);
			
			root=nary_delete(s,root);

			strcpy(s->location_full_path,string);
			fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
			fwrite(s,sizeof(struct file_descriptor),1,pFile);
		
			modified_fd[mfd_ptr]=occupied_fd[i];
			mfd_ptr++;

			insert_hashtable(block,s);
		}
	}
	
	fclose(pFile);

	pFile = fopen(vfs_name,"r+b");

	for(i=mfd_ptr-1; i>=0; i--)
	{
		fseek (pFile,sizeof(struct main_header) + modified_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		struct file_descriptor *s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);

		char *match = strstr (s->location_full_path,newpath);
		if(match!=NULL)
		{
			root=nary_insert(s,root);
		}
	}

	fclose(pFile);

	return error;

}

int mylistdir (char* dirpath, int flag, char* txtpath,struct ntree *root,int num_of_blocks,char* vfs_name)
{
	FILE *pFile;
	int *primary_freelist,*buffer;
	char *match;
	int match_non_rec = 1;
	int i=0,j=0;
	int error = 0;
	int wrongpath = 0;

	pFile = fopen(vfs_name,"r+b");
	fseek (pFile,sizeof(struct main_header)+sizeof(struct file_descriptor)*num_of_blocks,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*num_of_blocks);
	size_t result = fread(buffer,sizeof(int),num_of_blocks,pFile);
	fclose(pFile);
	if (buffer == NULL) {puts ("Memory error"); flag=0;}
	primary_freelist=(int *)buffer;

	int *occupied_fd=(int*) malloc (sizeof(int)*num_of_blocks);
	for(j=0; j<num_of_blocks; j++)
	{
		occupied_fd[j]=-1;
	}

	j=0;
	for(i=0;i<num_of_blocks;i++)
	{
		if(primary_freelist[i]==1)
		{
			occupied_fd[j++]=i;
		}
	}
	int size_fd = j-1;	

	pFile = fopen(vfs_name,"r+b");

	FILE *file;
	file=fopen(txtpath,"w");
	if(file!=NULL)
	{
		for(i=size_fd; i>=0; i--)
		{
			fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
			struct file_descriptor *s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
			result = fread (s,sizeof(struct file_descriptor),1,pFile);

			if(flag==1)
			{
				match = strstr (s->location_full_path,dirpath);
				if(match!=NULL)
				{
					char *dname = s->file_name;
					fwrite(dname,strlen(dname),1,file);
					char *nl = "\n";
					fwrite(nl,strlen(nl),1,file);
					wrongpath = 1;
				}
			}
			else if(flag==0)
			{
				match_non_rec = strcmp(s->location_full_path,dirpath);
				if(match_non_rec==0)
				{
					char *dname = s->file_name;
					fwrite(dname,strlen(dname),1,file);
					char *nl = "\n";
					fwrite(nl,strlen(nl),1,file);
					wrongpath = 1;
				}
			}
			else
			{
				//error in flag
				error = 2;
				return error;
			}
		}
	}
	else
	{
		//error in opening txt file
		error = 3;
		return error;
	}

	if(wrongpath ==0)
	{
		error = 1;
		return error;
	}
	fclose(file);
	fclose(pFile);

	return error;
}


int mydeletedir (char* path,struct ntree *root,struct hash_node block[26],int num_of_blocks,char* vfs_name)
{
	struct ntree *temp=root;
	char *fullpath=(char *)malloc(sizeof(char)*100);
	char str[100];
        char file_name[100];
  	char new[num_of_blocks][200];
  	char *pch;
	int i=0,j=0;
	int flag=0,notFound=0;
	int error=0;
  	
	strcpy(fullpath,path);

	pch = strtok (path,"/");
    if(pch==NULL)
    {
     error=1;
     return error;
     }
	while (pch != NULL)
  	{
    		strcpy(new[i],pch);
		i++;
    		pch = strtok (NULL, "/");
  	}
	if(i<=1)
	{
	error=1;
	return error;
	}
	char *newpath;
	char string[100]="";
	j=0;
	while(j<(i-1))
	{
		strcat(string,new[j]);
		newpath=strcat(string,"/");
		j++;
	}
        strcpy(file_name,new[i-1]);
	strcpy(newpath,string);
	int num_of_tokens = i;
        

       	FILE *pFile;
	int *primary_freelist,*buffer;
	pFile = fopen(vfs_name,"r+b");
	fseek (pFile,sizeof(struct main_header)+sizeof(struct file_descriptor)*num_of_blocks,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*num_of_blocks);
	size_t result = fread(buffer,sizeof(int),num_of_blocks,pFile);
	fclose(pFile);
	if (buffer == NULL) {puts ("Memory error"); flag=0;}
	primary_freelist=(int *)buffer;

	int *occupied_fd=(int*) malloc (sizeof(int)*num_of_blocks);
	for(j=0; j<num_of_blocks; j++)
	{
		occupied_fd[j]=-1;
	}

	j=0;
	for(i=0;i<num_of_blocks;i++)
	{
		if(primary_freelist[i]==1)
		{
			occupied_fd[j++]=i;
		}
	}
	int size_fd = j-1;	

	pFile = fopen(vfs_name,"r+b");
         
         char *match = NULL;
	
	for(i=0; i<=size_fd; i++)
	{
		fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		struct file_descriptor *s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);
                 
               
		if(strcmp(s->location_full_path,newpath)==0 && strcmp(s->file_name,file_name)==0)
		{
                  notFound=1;	
                }
		
              	
		
            }
               if(notFound==0)
               {
                      error = 1;	//CANNOT_FIND_SPECIFIED_DIR
	                return error;
               }
           for(i=0; i<=size_fd; i++)
	{
		fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		struct file_descriptor *s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);

		match = strstr (s->location_full_path,fullpath);
                
		
		if(match!=NULL)
		{
                  	error=2;	//DIRECTORY_IS_NOT_EMPTY
                  	return error;
                }

              	
		
            }
         
        match=NULL;
        struct file_descriptor *s;
        
       for(i=0; i<=size_fd; i++)
	{
		fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
		s = (struct file_descriptor*) malloc (sizeof(struct file_descriptor));
		result = fread (s,sizeof(struct file_descriptor),1,pFile);

		if(strcmp(s->location_full_path,newpath)==0 && strcmp(s->file_name,new[num_of_tokens-1])==0){
                         match="found";
			root=nary_delete(s,root);
			delete_hashtable(block,s);

			primary_freelist[occupied_fd[i]]=0;

			struct file_descriptor *fd=(struct file_descriptor*)malloc(sizeof(struct file_descriptor));
                        strcpy(fd->file_name,"");
			strcpy(fd->location_full_path,"");
			fd->file_type=0;
			fd->file_size=0;
			fd->location_block_number=0;
			fseek (pFile,sizeof(struct main_header) + occupied_fd[i]* sizeof(struct file_descriptor),SEEK_SET);
			fwrite(fd,sizeof(struct file_descriptor),1,pFile);
                        break;
		}
	}

	

        fseek (pFile,sizeof(struct main_header) + num_of_blocks*sizeof(struct file_descriptor),SEEK_SET);
	fwrite(primary_freelist,sizeof(int),num_of_blocks,pFile);
	fclose(pFile);
	
	return error;
}


int mymakedir (char* path, char* name,struct ntree *root,struct hash_node block[26],int num_of_blocks,char* vfs_name)
{       
	struct ntree *temp=root;
	char *savedpath=(char *)malloc(sizeof(char)*100);
	strcpy(savedpath,path);
	char str[100];
  	char new[num_of_blocks][200];
  	char *pch;
	int i=0;
	int k=0,empty_fd=0;
	int flag=0;
  	int error = 0;
	char *dirname =(char *)malloc(sizeof(char)*100);
	strcpy(dirname,name);
  	
	for(k = 0;k < strlen(dirname);k++){
		if(dirname[k] == 47){
			error = 2; //Error - Invalid character in filename
			return error;
		}
	}
       
        i=0;

	FILE *pFile;
	int *primary_freelist,*buffer;
	pFile = fopen(vfs_name,"r+b");
	fseek (pFile,sizeof(struct main_header)+sizeof(struct file_descriptor)*num_of_blocks,SEEK_SET);
	buffer = (int*)malloc(sizeof(int)*num_of_blocks);
	size_t result = fread(buffer,sizeof(int),num_of_blocks,pFile);
	fclose(pFile);

        primary_freelist=(int *)buffer;
        
  	i=0;
        pch = strtok (path,"/");
	while (pch != NULL)
  	{
    		strcpy(new[i],pch);
		i++;
    		pch = strtok (NULL, "/");
  	}
	
	char* path1="bond/";
	int j=0,check=0;
	k=0;
	for(j=1;j<i;j++)
	{
		int check = search_hashtable_diroper(block,new[j],path1);
		if(check==1)
		{
			for(k=0; k<num_of_blocks; k++)
			{
	           		if(primary_freelist[k]==0)
				{
					
					break;
				}
			}
                       
			if(k < num_of_blocks)
			{
				
                               int empty_fd = k;
                        
				//insert in nary and hash
				
				struct file_descriptor *fd=(struct file_descriptor*)malloc(sizeof(struct file_descriptor));
				strcpy(fd->file_name,new[j]);
				strcpy(fd->location_full_path,path1);
				fd->file_type=0;
				fd->file_size=0;
				fd->location_block_number=empty_fd;
				
				root=nary_insert(fd,root);
				insert_hashtable(block,fd);
			        
				FILE *stream;
				stream = fopen(vfs_name, "r+b");
				fseek (stream,sizeof(struct main_header)+(empty_fd*sizeof(struct file_descriptor)),SEEK_SET);
				fwrite(fd,sizeof(struct file_descriptor), 1, stream);
				fclose(stream);
				

				stream = fopen(vfs_name, "r+b");
				fseek (stream,sizeof(struct main_header)+sizeof(struct file_descriptor)*num_of_blocks,SEEK_SET);
				primary_freelist[empty_fd]=1;
				fwrite(primary_freelist,sizeof(int),num_of_blocks, stream);
				fclose(stream);
				
				check=0;
			}
			else
			{
			error = 1;
			return error;
		        }
			
			
		}
		char string[100];
		strcpy(string,path1);
		path1=strcat(string,new[j]);
		path1=strcat(string,"/");
                
	}

	for(k=0; k<num_of_blocks; k++)
	{
		if(primary_freelist[k]==0)
		{
			break;
		}
	}
	empty_fd=k;
	
	int c = search_hashtable_diroper(block,name,savedpath);
	if(empty_fd<num_of_blocks && c==1)
	{
		struct file_descriptor *fd=(struct file_descriptor*)malloc(sizeof(struct file_descriptor));
		strcpy(fd->file_name,name);
		strcpy(fd->location_full_path,savedpath);
		fd->file_type=0;
		fd->file_size=0;
		fd->location_block_number=empty_fd;
			
		nary_insert(fd,root);
		insert_hashtable(block,fd);

		FILE *stream;
		stream = fopen(vfs_name, "r+b");
		fseek (stream,sizeof(struct main_header)+(empty_fd*sizeof(struct file_descriptor)),SEEK_SET);
		fwrite(fd,sizeof(struct file_descriptor), 1, stream);
		fclose(stream);

		stream = fopen(vfs_name, "r+b");
		fseek (stream,sizeof(struct main_header)+(num_of_blocks*sizeof(struct file_descriptor)),SEEK_SET);
		primary_freelist[empty_fd]=1;
		fwrite(primary_freelist,sizeof(int),num_of_blocks, stream);
		fclose(stream);
		
		
                				
		
	}
	else
	{
		

		if(empty_fd>=num_of_blocks)
		{
			error = 1;
			return error;
		}
		else if(c!=1)
		{
			error = 3;
			return error;
		}
	}
	return error;
}


