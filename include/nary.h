struct ntree {

	char file_name[20];
	char location_full_path[100];
	int file_type;
	long file_size;
	long location_block_number;
	
	int left_traversed;

  	struct ntree *left;
  	struct ntree *sibling;
	struct ntree *parent;
};


struct ntree *root,*prev,*sib;

void print_ntree(struct ntree *root);
struct ntree* nary_insert (struct file_descriptor *node, struct ntree *root);
struct ntree* nary_delete(struct file_descriptor *node, struct ntree *root);
struct ntree* nary_insert_root();
