int mymakedir (char* path, char* name,struct ntree *root,struct hash_node block[26],int num_of_blocks,char* vfs_name);
int mydeletedir (char* path,struct ntree *root,struct hash_node block[26],int num_of_blocks,char* vfs_name);
int mymovedir ( char* oldpath, char* newpath, struct ntree *root,struct hash_node block[26],int num_of_blocks,char* vfs_name);
int mylistdir (char* dirpath, int flag, char* txtpath,struct ntree *root,int num_of_blocks,char* vfs_name);
