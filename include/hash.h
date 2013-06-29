struct hash_node{
	
	char file_name[20];
	char location_full_path[100];
	int file_type;
	long file_size;
	long location_block_number;

	struct hash_node *next;
};

void initialize_hashtable(struct hash_node block[]);
void insert_hashtable(struct hash_node block[],struct file_descriptor *node );
int search_hashtable(struct hash_node block[],char *name);
int check_duplicate(struct hash_node block[],struct file_descriptor *node);
void delete_hashtable(struct hash_node block[],struct file_descriptor *node);
int search_hashtable_diroper(struct hash_node block[],char *name,char *path);
