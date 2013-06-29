/*
 * File: vfsdriver.c
 *
 * Description: This is a driver program for testing your VFS system using an interaction script as input
 * You need to make additional calls to your respective functions as noted in the comments below
 * Make sure the output you display is exactly as per the given specifications for you. Do NOT print
 * any extra output (like debug messages) in the final version of your driver program. You can use this driver program
 * in a in incremental manner as you keep implementing one operator after another. For operators not yet implemented,
 * you can leave the output as given ("TO_BE_DONE"). So you can use this program as your "main" program for testing purposes.
 *
 * DO NOT write the full code for operators in the driver program! You must only CALL your functions from here.
 *
 * Usage: vfsdriver <scriptfilename>
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "../include/vfs_errorcodes.h"
#include "../include/vfs_main.h"
#include "../include/nary.h"
#include "../include/hash.h"
#include "../include/create_vfs.h"
#include "../include/mount.h"



#define BUFSIZE 200
#define CMDSIZE 30
#define PARSIZE 100

void createvfs ( char *P1, int P2 );
void mountvfs ( char *P1 );
void unmountvfs ( char *P1 );
void makedir ( char *P1, char *P2 );
void deletedir ( char *P1 );
void movedir ( char *P1, char *P2 );
void listdir ( char *P1, int P2, char *P3 );
void addfile ( char *P1, char *P2, char *P3 );
void listfile ( char *P1, char *P2 );
void updatefile ( char *P1, char *P2 );
void removefile ( char *P1 );
void movefile ( char *P1, char *P2 );
void copyfile ( char *P1, char *P2 );
void exportfile ( char *P1, char *P2 );
void searchfile ( char *P1, char *P2 );
int num_of_blocks;
int vfs_size;
char* vfs_name;
char path[2000];
char temp[2000];
struct hash_node block[27];
struct ntree *root;
int mounted = 0;

void processcommand( char *command, char *P1, char *P2, char *P3 );

int main( int argc, char *argv[] )
{
	FILE *scriptfp;
	char linebuffer[BUFSIZE];
	char command[CMDSIZE], par1[PARSIZE], par2[PARSIZE], par3[PARSIZE];
	char *token;
	

	if( argc != 2 ){
		fprintf(stderr,"Usage: vfsdriver <scriptfile>\n");
		return(1);
	}

	if( (scriptfp=fopen(argv[1],"r")) == NULL ){
		fprintf(stderr,"Unable to open script file: %s\n", argv[1]);
		return(2);
	}

	while( fgets(linebuffer, sizeof(linebuffer), scriptfp) != NULL ){
		
		//printf("==================================================================================================\n");
		//printf("Processing: %s", linebuffer);
		//printf("==================================================================================================\n");
		

		/* Remove the extra newline character in the end of line */
		linebuffer[ strlen(linebuffer)-1 ] = '\0';
		
		/* Get the command and the parameters using tokenizer */
		strcpy( command, (token = strtok(linebuffer, " ")) == NULL ? "" : token );
		/*if(strcmp(command,"updatefile"==0){
                  strcpy( par1, (token = strtok(NULL, " ")) == NULL ? "" : token );
                  strcpy( par2, (token = strtok(NULL, "\n")) == NULL ? "" : token );
                   }
                 else{}
                 */
		strcpy	( par1, (token = strtok(NULL, " ")) == NULL ? "" : token );
		strcpy( par2, (token = strtok(NULL, " ")) == NULL ? "" : token );
		strcpy( par3, (token = strtok(NULL, " ")) == NULL ? "" : token );
		/* printf("Command:%s:p1:%s:p2:%s:p3:%s\n",command, par1, par2, par3); */

		processcommand( command, par1, par2, par3 );
	}
	if(mounted==1)
	{
		char *P1=(char *)malloc(sizeof(char)*100); 
		strcpy(P1,vfs_name);
		strcat(P1,"_copy");
		remove(vfs_name);
		rename(P1,vfs_name);
	}
}

void processcommand( char *command, char *P1, char *P2, char *P3 )
{
	if( strcmp(command, "createvfs") == 0 ){
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("createvfs_FAILURE %s\n",ERR_VFS_CREATE_00);
		}
		else
		{
			int size = atoi(P2)*1000;
			createvfs (P1,size);
		}
		
	}	
	else if( strcmp(command, "mountvfs") == 0 ) {

		if(strcmp(P1,"")==0)
		{
			printf("mountvfs_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{

			FILE *pFile;
			pFile = fopen(P1,"r+b");
			if (pFile==NULL) 
			{
				//printf("mountvfs_TO_BE_DONE\n");
				printf("mountvfs_FAILURE DATA_FILE_NOT_FOUND\n");
	
			}
			else
			{
				
				struct main_header* mho=(struct main_header*) malloc (sizeof(struct main_header));
				size_t result=fread(mho,sizeof(struct main_header),1,pFile);
				vfs_size=mho->max_file_descriptor*sizeof(struct block);
				vfs_name = mho->file_system_label;
		
				fclose(pFile);
				num_of_blocks = vfs_size/sizeof(struct block);
				mountvfs(P1);
			}
		}
	}
	else if( strcmp(command, "unmountvfs") == 0 ){
		if(strcmp(P1,"")==0)
		{
			printf("unmountvfs_FAILURE %s\n",ERR_VFS_MOUNT_00);
			/*if(strcmp(P1,vfs_name)==0)
			{
				strcpy(P1,vfs_name);
				strcat(P1,"_copy");
				remove(vfs_name);
				rename(P1,vfs_name);
			}*/
		}
		else
		{
			unmountvfs(P1);
		}
	}
	else if( strcmp(command, "makedir") == 0 ){
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("makedir_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{
			if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P1,path);  
			}
			

			
			makedir (P1,P2);
		}
	}
	else if( strcmp(command, "deletedir") == 0 ){
		if(strcmp(P1,"")==0)
		{
			printf("deletedir_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{       if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                       		strcpy(P1,path);  
			}
			

			deletedir (P1);
		}
	}
	else if( strcmp(command, "movedir") == 0 ){
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("movedir_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{	if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		strcpy(P1,path);  
			}
                      

                      if(P2[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P2);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P2,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P2);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P2,path);  
			}
                       
			
			movedir (P1,P2);
		}
	}
	else if( strcmp(command, "listdir") == 0 ){
		if(strcmp(P1,"")==0||strcmp(P2,"")==0||strcmp(P3,"")==0)
		{
			printf("listdir_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{  
			if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P1,path);  
			}  
                       
			int flag = atoi(P2);
			listdir (P1,flag,P3);
		}
	}
	else if( strcmp(command, "addfile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0||strcmp(P3,"")==0)
		{
			printf("addfile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{       
            if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  								strcat(path,"/");
                       		strcpy(P1,path);  
			}
			else
			{
							strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  								strcat(path,"/");
                       		strcpy(P1,path);  
			}
                  

                       addfile(P1,P2,P3);
		}
	}
	else if( strcmp(command, "listfile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("listfile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{       
                       if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      	
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		
                       		strcpy(P1,path);  
			}
                        
			
                     
                   
			listfile (P1,P2);
		}
	}
	else if( strcmp(command, "updatefile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("updatefile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{
                       if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      	
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		
                       		strcpy(P1,path);  
			}
                      
                      
                      
			updatefile (P1,P2);
		}
	}
	else if( strcmp(command, "removefile") == 0 )
	{
		if(strcmp(P1,"")==0)
		{
			printf("removefile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{	
                         if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      	
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		
                       		strcpy(P1,path);  
			}
          
			removefile (P1);
		}
	}
	else if( strcmp(command, "movefile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("movefile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{       
                         if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      	
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		
                       		strcpy(P1,path);  
			}
                        

                            if(P2[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P2);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P2,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P2);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P2,path);  
			}
                   
			movefile (P1,P2);
		}
	}
	else if( strcmp(command, "copyfile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("copyfile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else

         	{ 
                      if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      	
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		
                       		strcpy(P1,path);  
			}
                      
                        
                       if(P2[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P2);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P2,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P2);
                      		strcat(path,temp);
                      		if(path[strlen(path)-1]!='/')
  					strcat(path,"/");
                       		strcpy(P2,path);  
			}
                    
			copyfile (P1,P2);
		}
	}
	else if( strcmp(command, "exportfile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("exportfile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{	     
                             	if(P1[0]!='/')
			{
                      		strcpy(path,"bond/");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      	
                       		strcpy(P1,path);  
			}
			else
			{
				strcpy(path,"bond");
                      		strcpy(temp,P1);
                      		strcat(path,temp);
                      		
                       		strcpy(P1,path);  
			}
                             
                       

                        	exportfile (P1,P2);
		}
	}
	else if( strcmp(command, "searchfile") == 0 )
	{
		if(strcmp(P1,"")==0||strcmp(P2,"")==0)
		{
			printf("searchfile_FAILURE %s\n",ERR_VFS_MOUNT_00);
		}
		else
		{             
                       
                       
			searchfile (P1,P2);
		}
	}
	else
	{
		printf("Ignoring invalid command %s\n", command);
	}
}

void createvfs ( char *P1, int P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("createvfs_TO_BE_DONE\n");
	int i = create_vfs(P1,P2);

	switch(i)
	{
		case 0: printf("createvfs_SUCCESS\n"); break;
		case 1: printf("createvfs_FAILURE %s\n",ERR_VFS_CREATE_01); break;
		case 2: printf("createvfs_FAILURE %s\n",ERR_VFS_CREATE_02); break;
		case 3: printf("createvfs_FAILURE %s\n",ERR_VFS_CREATE_03); break;
		case 4: printf("createvfs_FAILURE %s\n",ERR_VFS_CREATE_04); break;
		case 5: printf("createvfs_FAILURE %s\n",ERR_VFS_CREATE_05); break;
		default:printf("\n");
		//default : printf("createvfs_FAILURE Unknown Failure\n");
	}
}

void mountvfs ( char *P1 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("mountvfs_TO_BE_DONE\n");
	int i;
	if(mounted==1)
	{
		i=4;
	}
	else
	{
		root=nary_insert_root();
		i=mount(P1,root,block);
	}
        switch(i)
	{
		case 0: printf("mountvfs_SUCCESS\n"); break;
		case 1: printf("mountvfs_FAILURE %s\n",ERR_VFS_MOUNT_01); break;
		case 2: printf("mountvfs_FAILURE %s\n",ERR_VFS_MOUNT_02); break;
		case 3: printf("mountvfs_FAILURE %s\n",ERR_VFS_MOUNT_03); break;
		case 4: printf("mountvfs_FAILURE %s\n",ERR_VFS_MOUNT_04); break;
		default:printf("\n");
		//default : printf("mountvfs_FAILURE Unknown Failure\n");
	}

	if(i==0)
	{
		mounted=1;
		strcat(P1,"_copy");
		int i = copy_vfs(vfs_name,P1,vfs_size);
	}
}

void unmountvfs ( char *P1 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("unmountvfs_TO_BE_DONE\n");
	int i ;
	if(mounted==0)
	{
		i=4;
	}
	else
	{
		i= unmount(P1,vfs_name);
	};
	switch(i)
	{
		case 0: printf("unmountvfs_SUCCESS\n"); break;
		case 1: printf("unmountvfs_FAILURE %s\n",ERR_VFS_UNMOUNT_01); break;
		case 2: printf("unmountvfs_FAILURE %s\n",ERR_VFS_UNMOUNT_02); break;
		case 3: printf("unmountvfs_FAILURE %s\n",ERR_VFS_UNMOUNT_03); break;
		case 4: printf("unmountvfs_FAILURE %s\n",ERR_VFS_UNMOUNT_04); break;
		default:printf("\n");
		//default : printf("makedir_FAILURE Unknown Failure\n");
	}
	
	if(i==0)
	{       
		strcat(P1,"_copy");
		remove(P1);
		mounted=0;
	}
	else if(i!=4)
	{
		if(strcmp(P1,vfs_name)==0)
		{
			strcat(P1,"_copy");
			remove(vfs_name);
			rename(P1,vfs_name);
		}
	}
}

void makedir ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("makedir_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=5;
	}
	else
	{
		num_of_blocks = vfs_size/sizeof(struct block);
		i = mymakedir(P1,P2,root,block,num_of_blocks,vfs_name);
	}
	
	switch(i)
	{
		case 0: printf("makedir_SUCCESS\n"); break;
		case 1: printf("makedir_FAILURE %s\n",ERR_VFS_MAKEDIR_01); break;
		case 2: printf("makedir_FAILURE %s\n",ERR_VFS_MAKEDIR_02); break;
		case 3: printf("makedir_FAILURE %s\n",ERR_VFS_MAKEDIR_03); break;
		case 5: printf("makedir_FAILURE %s\n",ERR_VFS_MAKEDIR_05); break;
		default:printf("\n");
		//default : printf("makedir_FAILURE Unknown Failure\n");
	}

	
}

void deletedir ( char *P1 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("deletedir_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=4;
	}
	else
	{
		i = mydeletedir(P1,root,block,num_of_blocks,vfs_name);
	}
	switch(i)
	{
		case 0: printf("deletedir_SUCCESS\n"); break;
		case 1: printf("deletedir_FAILURE %s\n",ERR_VFS_DELETEDIR_01); break;
		case 2: printf("deletedir_FAILURE %s\n",ERR_VFS_DELETEDIR_02); break;
		
		case 4: printf("deletedir_FAILURE %s\n",ERR_VFS_DELETEDIR_04); break;
		default:printf("\n");
		//default : printf("deletedir_FAILURE Unknown Failure\n");
	}
}

void movedir ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("movedir_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=8;
	}
	else
	{
		
		i=mymovedir(P1,P2,root,block,num_of_blocks,vfs_name);
	}
	switch(i)
	{
		case 0: printf("movedir_SUCCESS\n"); break;
		case 1: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_01); break;
		case 2: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_02); break;
		
		case 4: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_04); break;
		case 5: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_05); break;
		case 6: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_06); break;
		case 7: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_07); break;
		case 8: printf("movedir_FAILURE %s\n",ERR_VFS_MOVEDIR_08); break;
		default:printf("\n");
		//default : printf("movedir_FAILURE Unknown Failure\n");
	}

}

void listdir ( char *P1, int P2, char *P3 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("listdir_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=4;
	}
	else
	{
		i=mylistdir (P1,P2,P3,root,num_of_blocks,vfs_name);
	}
	switch(i)
	{
		case 0: printf("listdir_SUCCESS\n"); break;
		case 1: printf("listdir_FAILURE %s\n",ERR_VFS_LISTDIR_01); break;
		case 2: printf("listdir_FAILURE %s\n",ERR_VFS_LISTDIR_02); break;
		case 3: printf("listdir_FAILURE %s\n",ERR_VFS_LISTDIR_03); break;
		case 4: printf("listdir_FAILURE %s\n",ERR_VFS_LISTDIR_04); break;
		default:printf("\n");
		//default : printf("listdir_FAILURE Unknown Failure\n");
	}
}

void addfile ( char *P1, char *P2, char *P3 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("addfile_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=7;
	}
	else
	{
		
		i = My_addfile(P1,P2,P3,vfs_size,vfs_name,root,block);
	}

	switch(i)
	{
		case 0: printf("addfile_SUCCESS\n"); break;
		case 2: printf("addfile_FAILURE %s\n",ERR_VFS_ADDFILE_02); break;
		case 3: printf("addfile_FAILURE %s\n",ERR_VFS_ADDFILE_03); break;
		case 4: printf("addfile_FAILURE %s\n",ERR_VFS_ADDFILE_04); break;
		case 5: printf("addfile_FAILURE %s\n",ERR_VFS_ADDFILE_05); break;
		case 6: printf("addfile_FAILURE %s\n",ERR_VFS_ADDFILE_06); break;
		case 7: printf("addfile_FAILURE %s\n",ERR_VFS_ADDFILE_07); break;
		default:printf("\n");
		//default : printf("addfile_FAILURE Unknown Failure\n");
	}
}

void listfile ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("listfile_TO_BE_DONE\n");
	int i;

	if(mounted==0)
	{
		i=4;
	}
	else
	{
		i = My_listFile(P1,P2,vfs_size,vfs_name,root,block);
	}
	switch(i)
	{
		case 0: printf("listfile_SUCCESS\n"); break;
		case 1: printf("listfile_FAILURE %s\n",ERR_VFS_LISTFILE_01); break;
		case 2: printf("listfile_FAILURE %s\n",ERR_VFS_LISTFILE_02); break;
		case 3: printf("listfile_FAILURE %s\n",ERR_VFS_LISTFILE_03); break;
		case 4: printf("listfile_FAILURE %s\n",ERR_VFS_LISTFILE_04); break;
		default:printf("\n");
		//default : printf("listfile_FAILURE Unknown Failure\n");
	}
}

void updatefile ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("updatefile_TO_BE_DONE\n");
	int i ;
	if(mounted==0)
	{
		i=4;
	}
	else
	{
		i= My_updateFile(P1,P2,vfs_size,vfs_name,root,block);
	}
	switch(i)
	{
		case 0: printf("updatefile_SUCCESS\n"); break;
		case 1: printf("updatefile_FAILURE %s\n",ERR_VFS_UPDATEFILE_01); break;
		case 2: printf("updatefile_FAILURE %s\n",ERR_VFS_UPDATEFILE_02); break;
		case 3: printf("updatefile_FAILURE %s\n",ERR_VFS_UPDATEFILE_03); break;
		default:printf("\n");
		//default : printf("updatefile_FAILURE Unknown Failure\n");
	}
}

void removefile ( char *P1 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("removefile_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=2;
	}
	else
	{
		i = My_removeFile(P1,vfs_size,vfs_name,root,block);
	}
	switch(i)
	{
		case 0: printf("removefile_SUCCESS\n"); break;
		case 1: printf("removefile_FAILURE %s\n",ERR_VFS_REMOVEFILE_01); break;
		case 2: printf("removefile_FAILURE %s\n",ERR_VFS_REMOVEFILE_02); break;
		default:printf("\n");
		//default : printf("removefile_FAILURE Unknown Failure\n");
	}
}

void movefile ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("movefile_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=6;
	}
	else
	{
		i = My_moveFile(P1,P2,vfs_size,vfs_name,root,block);
	}
	switch(i)
	{
		case 0: printf("movefile_SUCCESS\n"); break;
		case 1: printf("movefile_FAILURE %s\n",ERR_VFS_MOVEFILE_01); break;
		case 2: printf("movefile_FAILURE %s\n",ERR_VFS_MOVEFILE_02); break;
		case 6: printf("movefile_FAILURE %s\n",ERR_VFS_MOVEFILE_06); break;
		default:printf("\n");
		//default : printf("movefile_FAILURE Unknown Failure\n");
	}
}

void copyfile ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("copyfile_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=5;
	}
	else
	{
		i = My_copyFile(P1,P2,vfs_size,vfs_name,root,block);
	}
	switch(i)
	{
		case 0: printf("copyfile_SUCCESS\n"); break;
		case 1: printf("copyfile_FAILURE %s\n",ERR_VFS_COPYFILE_01); break;
		case 2: printf("copyfile_FAILURE %s\n",ERR_VFS_COPYFILE_02); break;
		case 3: printf("copyfile_FAILURE %s\n",ERR_VFS_COPYFILE_03); break;
		case 4: printf("copyfile_FAILURE %s\n",ERR_VFS_COPYFILE_04); break;
		case 5: printf("copyfile_FAILURE %s\n",ERR_VFS_COPYFILE_05); break;
		default:printf("\n");
		//default : printf("copyfile_FAILURE Unknown Failure\n");
	}
}

void exportfile ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("exportfile_TO_BE_DONE\n");
	int i ;
	if(mounted==0)
	{
		i=4;
	}
	else
	{
		i= My_exportFile(P1,P2,vfs_size,vfs_name,root,block);
	}
	switch(i)
	{
		case 0: printf("exportfile_SUCCESS\n"); break;
		case 1: printf("exportfile_FAILURE %s\n",ERR_VFS_EXPORTFILE_01); break;
		case 2: printf("exportfile_FAILURE %s\n",ERR_VFS_EXPORTFILE_02); break;
		case 3: printf("exportfile_FAILURE %s\n",ERR_VFS_EXPORTFILE_03); break;
		case 4: printf("exportfile_FAILURE %s\n",ERR_VFS_EXPORTFILE_04); break;
		default:printf("\n");
		//default : printf("exportfile_FAILURE Unknown Failure\n");
	}
}

void searchfile ( char *P1, char *P2 )
{
	/* Call the appropriate function with given arguments and display appropriate output on the screen */
	//printf("searchfile_TO_BE_DONE\n");
	int i;
	if(mounted==0)
	{
		i=2;
	}
	else
	{
		i = My_searchFile(P1,P2,vfs_size,vfs_name,root,block);
	}
	if(i==2)
	{
		printf("searchfile_FAILURE %s\n",ERR_VFS_SEARCHFILE_02);
	}
}
