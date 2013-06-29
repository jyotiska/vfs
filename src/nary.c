#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>

#include "../include/vfs_main.h"
#include "../include/nary.h"



struct ntree* nary_insert (struct file_descriptor *node, struct ntree *root)
{
	struct ntree *temp=root;
 	int rootcheck = 0;
	int i = 0,pos=0,j=0,y,z;
	int fail_cause=-1;

	char str[100];
  	char new[10][20];
  	char * pch;
   	strcpy(str,node->location_full_path);
  	pch = strtok (str,"/");
  	
	while (pch != NULL)
  	{
    		strcpy(new[i],pch);
		i++;
    		pch = strtok (NULL, "/");
	
  	}
	while(temp)
	{
		

		if(rootcheck == 0)
		{
		
			if(strcmp(temp->file_name,new[j])==0)
			{
				prev=temp;
				temp=temp->left;
                                j++;
			}
			else
			{
				
				fail_cause = 0;
			}
			rootcheck=1;
		}
		
		
		if(temp!=NULL)
		{
			if(fail_cause==-1 && j!=i && strcmp(temp->file_name,new[j])==0)
			{
				prev=temp;
				temp=temp->left;
				j++;
	
			}
			else
			{
				sib=temp;
				temp=temp->sibling;
			}
		}
	}
	if(fail_cause == -1)
	{
		if(prev->left==NULL)
		{
	//create
			struct ntree *new=(struct ntree*)malloc(sizeof(struct ntree));
			new->left_traversed=0;
			new->parent=prev;
			prev->left=new;
			strcpy(new->file_name,node->file_name);
			new->left=NULL;
			new->sibling=NULL;

			strcpy(new->location_full_path,node->location_full_path);
			new->file_type=node->file_type;
			new->file_size=node->file_size;
			new->location_block_number = node->location_block_number;

		}
		else if(strcmp(prev->file_name,new[i-1])==0)
		{
			struct ntree *new=(struct ntree*)malloc(sizeof(struct ntree));
			new->left_traversed=0;
			new->parent=prev;
			sib->sibling=new;
			strcpy(new->file_name,node->file_name);
			new->left=NULL;
			new->sibling=NULL;

			strcpy(new->location_full_path,node->location_full_path);
			new->file_type=node->file_type;
			new->file_size=node->file_size;
			new->location_block_number = node->location_block_number;
		}
		
	}
	return root;
}

struct ntree* nary_delete(struct file_descriptor *node,struct ntree *root)
{
	struct ntree *temp=root;
	struct ntree *prev;
	int i = 0,j=0;
	char str[100];
	char new[10][20];
  	char * pch;
	
	int pathcheck = 0;
	
 
  	strcpy(str,node->location_full_path);

  	pch = strtok (str,"/");
  	while (pch != NULL)
  	{
    		strcpy(new[i],pch);
		i++;
    		pch = strtok (NULL, "/");
  	}
	int num_of_tokens = i;

	int ret_val = 0;
	for(i=0; i<num_of_tokens && temp!=NULL ; i++)
	{
		if(strcmp(new[i],temp->file_name)==0)
		{
			prev=temp;
			//sib=temp->sibling;
			temp=temp->left;
		}
		else
		{
			temp=temp->sibling;
			
			while(temp)
			{
				if(strcmp(new[i],temp->file_name)==0)
				{
					prev=temp;
					//sib=temp->sibling;
					temp=temp->left;
					break;
				}
				else
				{
					temp=temp->sibling;
				}
			}
		}
	}

	if(temp==NULL)
	{
		pathcheck=1;
	}

	struct ntree *prevsib=NULL;

	if(pathcheck==0)
	{

		if(strcmp(node->file_name,temp->file_name)==0)
		{
			
		}
		else
		{
			prevsib=temp;
			temp=temp->sibling;
			
			while(temp!=NULL)
			{
				if(strcmp(node->file_name,temp->file_name)==0)
				{
					break;
				}
				else
				{
					prevsib=temp;
					temp=temp->sibling;
				}
			}
		}
	}

	if(pathcheck == 0 )
	{
		if(prevsib!=NULL)
		{
			if(temp->sibling!=NULL)
			{
				prevsib->sibling = temp->sibling;
			}
			else
			{
				prevsib->sibling=temp->left;
			}
		}
		else if(temp->sibling != NULL)
		{
			prev->left=temp->sibling;
		}
		else
		{
			prev->left=temp->left;
		} 
	}
	return root;
}

struct ntree* nary_insert_root()
{
	
	struct ntree *root = (struct ntree*)malloc(sizeof(struct ntree));
	root->left_traversed=0;
	strcpy(root->file_name,"bond");
	root->left=NULL;
	root->sibling=NULL;
	root->parent=NULL;

	return root;
}

