typedef struct file_descriptor
{
	char file_name[20];
	char location_full_path[100];
	int file_type;		//dir-0  file-1
	long file_size;
	long location_block_number;
}*fd;

typedef struct free_list
{
	long block_number;
}*fl; 

typedef struct main_header
{
	char file_system_label[100];
	long max_file_descriptor;
	long used_file_descriptor;
	struct file_descriptor *file_descriptor_array;
	struct free_list *free_list_array;
}*mh;

typedef struct block
{
	char size_buffer_block[1000];
}*b;

void print_hello(void);
