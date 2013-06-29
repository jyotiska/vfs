#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include "../include/vfs_main.h"
#include "../include/hash.h"


void initialize_hashtable(struct hash_node block[])
{
	int i=0;
	char ch;
	for(i=0; i<27; i++)
	{
		ch = i+65;
		block[i].file_name[0] = ch;
		block[i].next=NULL;
	}
}

void insert_hashtable(struct hash_node block[],struct file_descriptor *node )
{
	struct hash_node *new=(struct hash_node*)malloc(sizeof(struct hash_node));
	struct hash_node *temp=(struct hash_node*)malloc(sizeof(struct hash_node));

	strcpy(new->file_name,node->file_name);
	strcpy(new->location_full_path,node->location_full_path);
	new->location_block_number = node->location_block_number;
	new->next=NULL;
	int pos;
	char first = node->file_name[0];
	if((first >= 65 && first <= 90) || (first >= 97 && first <= 122)){
		first = toupper(first);
		pos = first - 65;
	}
	else
		pos = 26;

	if(check_duplicate(block,node))
	{
		if(block[pos].next==NULL)
		{
			block[pos].next=new;
		}
		else
		{
			new->next=block[pos].next;
			block[pos].next=new;
		}
	}
}
int search_hashtable(struct hash_node block[],char *name)
{
	struct hash_node *temp=(struct hash_node*)malloc(sizeof(struct hash_node));
	struct hash_node *prev=(struct hash_node*)malloc(sizeof(struct hash_node));

	int flag = 1;
	int pos;
	  
	char first = name[0];
         
	if((first >= 65 && first <= 90) || (first >= 97 && first <= 122)){
		first = toupper(first);
		pos = first - 65;
	}
	else
		pos = 26;

	temp=block[pos].next;
	prev=&block[pos];

	if(block[pos].next==NULL)
	{
		flag=1;
	}
	else
	{
		while(temp)
		{
			if(strcmp(temp->file_name,name)==0)
			{
				flag=0;
				temp= temp->next;
			}
			else
			{
				prev=temp;
				temp=temp->next;
			}
		}
	}
	return flag;
}

long search_hashtable_path(struct hash_node block[],char *name, char *path)
{
	struct hash_node *temp=(struct hash_node*)malloc(sizeof(struct hash_node));
	struct hash_node *prev=(struct hash_node*)malloc(sizeof(struct hash_node));
	long flag=999;
	int pos;
	char first = name[0];
	if((first >= 65 && first <= 90) || (first >= 97 && first <= 122)){
		first = toupper(first);
		pos = first - 65;
	}
	else
		pos = 26;

	temp=block[pos].next;
	prev=&block[pos];

	if(block[pos].next==NULL)
	{
		flag = 999;
	}
	else
	{
		while(temp != NULL)
		{	
			if(strcmp(temp->file_name,name)==0 && strcmp(temp->location_full_path,path)==0)
			{
					flag = temp->location_block_number;
					temp= temp->next;
					return flag;
			}
			else
			{
				prev=temp;
				temp=temp->next;
			}
		}
	}
	return flag;
}


int search_hashtable_diroper(struct hash_node block[],char *name,char *path)
{
	struct hash_node *temp=(struct hash_node*)malloc(sizeof(struct hash_node));
	struct hash_node *prev=(struct hash_node*)malloc(sizeof(struct hash_node));
	
	int flag = 1;
	int pos;
	char first = name[0];
	 
	if((first >= 65 && first <= 90) || (first >= 97 && first <= 122)){
		first = toupper(first);
		pos = first - 65;
	}
	else
		pos = 26;
         
	temp=block[pos].next;
	prev=&block[pos];

	if(temp==NULL)
	{       
		flag=1;
	}
	else
	{
		while(temp)
		{
			if(strcmp(temp->file_name,name)==0 && strcmp(temp->location_full_path,path)==0)
			{
				flag=0;
				break;
			
			}
			else
			{
				prev=temp;
				temp=temp->next;
			}
		}
	}
	return flag;
}

int check_duplicate(struct hash_node block[],struct file_descriptor *node)
{
	struct hash_node *temp=(struct hash_node*)malloc(sizeof(struct hash_node));
	struct hash_node *prev=(struct hash_node*)malloc(sizeof(struct hash_node));

	int flag = 1;
	int pos;
	char first = node->file_name[0];
	if((first >= 65 && first <= 90) || (first >= 97 && first <= 122)){
		first = toupper(first);
		pos = first - 65;
	}
	else
		pos = 26;

	temp=block[pos].next;
	prev=&block[pos];

	if(block[pos].next==NULL)
	{
		return 1;
	}
	else
	{
		while(temp)
		{
			if(strcmp(temp->file_name,node->file_name)==0 && strcmp(temp->location_full_path,node->location_full_path)==0)
			{
				flag = 0;
				return 0;
				
			}
			else
			{
				prev=temp;
				temp=temp->next;
			}
		}
		if(flag==1)
		{
			return 1;
		}
	}
}

void delete_hashtable(struct hash_node block[],struct file_descriptor *node)
{
	struct hash_node *temp=(struct hash_node*)malloc(sizeof(struct hash_node));
	struct hash_node *prev=(struct hash_node*)malloc(sizeof(struct hash_node));

	int flag = 1;
	int pos;
	char first = node->file_name[0];
	if((first >= 65 && first <= 90) || (first >= 97 && first <= 122)){
		first = toupper(first);
		pos = first - 65;
	}
	else
		pos = 26;

	temp=block[pos].next;
	prev=&block[pos];

	if(block[pos].next==NULL)
	{
		flag=1;
	}
	else
	{
		while(temp)
		{
			if(strcmp(temp->file_name,node->file_name)==0 && strcmp(temp->location_full_path,node->location_full_path)==0)
			{
				prev->next=temp->next;
				free(temp);
				flag=0;
				break;
			}
			else
			{
				prev=temp;
				temp=temp->next;
			}
		}
	}
}
