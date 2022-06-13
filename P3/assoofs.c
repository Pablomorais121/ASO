#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include "assoofs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pablo Morais Alvarez");

//prototipos que he escrito yo===========================================
extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);
static struct dentry *assoofs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data);
extern struct dentry *mount_bdev(struct file_system_type *fs_type, int flags, const char *dev_name, void *data, int (*fill_super)(struct super_block *, void *, int));
int assoofs_fill_super(struct super_block *sb, void *data, int silent);
extern struct inode *new_inode(struct super_block *sb);
struct assoofs_inode_info *assoofs_get_inode_info(struct super_block *sb, uint64_t inode_no);
static struct inode *assoofs_get_inode(struct super_block *sb,int ino);
int assoofs_sb_get_a_freeblock(struct super_block *sb, uint64_t *block);
void assoofs_add_inode_info(struct super_block *sb, struct assoofs_inode_info *inode);
int assoofs_save_inode_info(struct super_block *sb, struct assoofs_inode_info *inode_info);
void assoofs_save_sb_info(struct super_block *vsb);
struct assoofs_inode_info *assoofs_search_inode_info(struct super_block *sb, struct assoofs_inode_info *start, struct assoofs_inode_info *search);
static int assoofs_iterate(struct file *filp, struct dir_context *ctx);
//prototipos parte opcional==============================================
static struct kmem_cache *assoofs_inode_cache;
void assoofs_destroy_inode(struct inode *inode);

static DEFINE_MUTEX(assoofs_sb_lock);
int retM;

void assoofs_remove();
void assoofs_move();
//=======================================================================
//Operaciones sobre ficheros

ssize_t assoofs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos);
ssize_t assoofs_write(struct file *filp, const char __user * buf, size_t len, loff_t *ppos);
const struct file_operations assoofs_file_operations = {
	.read = assoofs_read,
	.write = assoofs_write,
};

ssize_t assoofs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos){
	struct assoofs_inode_info *inode_info = filp->f_path.dentry->d_inode->i_private;
	struct buffer_head *bh;
	char *buffer;
	int nbytes;
	printk(KERN_INFO "Read request\n");
	
	if (*ppos >= inode_info->file_size) return 0;
	
	bh = sb_bread(filp->f_path.dentry->d_inode->i_sb, inode_info->data_block_number);
	buffer = (char *)bh->b_data;
	
	nbytes = min((size_t) inode_info->file_size,len);
	if(copy_to_user(buf,buffer,nbytes) != 0) return -1;
	
	
	*ppos += nbytes;
	return nbytes;
}

ssize_t assoofs_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos){
	struct assoofs_inode_info *inode_info = filp->f_path.dentry->d_inode->i_private;
	struct buffer_head *bh;
	struct super_block *sb = filp->f_path.dentry->d_inode->i_sb;
	char *buffer;
	printk(KERN_INFO "Write request\n");
	
	if(*ppos >= ASSOOFS_DEFAULT_BLOCK_SIZE) return 0;
	
	bh = sb_bread(filp->f_path.dentry->d_inode->i_sb, inode_info->data_block_number);
	
	buffer = (char *)bh->b_data;
	buffer += *ppos;
	if(copy_from_user(buffer,buf,len) != 0) return -1;
	
	*ppos+=len;
	
	 retM = mutex_lock_interruptible(&assoofs_sb_lock);
	 if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	mutex_unlock(&assoofs_sb_lock);
	
	inode_info->file_size = *ppos;
	assoofs_save_inode_info(sb,inode_info);
	printk(KERN_INFO "Paso el save");
	return len;
}

//Operaciones sobre directorios

static int assoofs_iterate(struct file *filp, struct dir_context *ctx);
const struct file_operations assoofs_dir_operations = {
	.owner = THIS_MODULE,
	.iterate = assoofs_iterate,
};

static int assoofs_iterate(struct file *filp, struct dir_context *ctx){
	struct inode *inode;
	struct super_block *sb;
	struct buffer_head *bh;
	struct assoofs_inode_info *inode_info;
	struct assoofs_dir_record_entry *record;
	int i;
	printk(KERN_INFO "Iterate request\n");
	
	
	inode = filp->f_path.dentry->d_inode;
	sb = inode->i_sb;
	inode_info = inode ->i_private;
	
	if(ctx->pos) return 0;
	
	if((!S_ISDIR(inode_info->mode))) return -1;
	
	bh = sb_bread(sb,inode_info->data_block_number);
	record = (struct assoofs_dir_record_entry *)bh->b_data;
	for(i = 0; i < inode_info->dir_children_count; i++){
		dir_emit(ctx, record->filename, ASSOOFS_FILENAME_MAXLEN, record->inode_no,DT_UNKNOWN);
		ctx->pos += sizeof(struct assoofs_dir_record_entry);
		record++;
	}
	brelse(bh);
	return 0;
}

//Operaciones sobre inodos

static int assoofs_create(struct user_namespace *mnt_userns, struct inode *dir, struct dentry *dentry, umode_t mode, bool excl);
struct dentry *assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags);
static int assoofs_mkdir(struct user_namespace *mnt_userns, struct inode *dir, struct dentry *dentry,umode_t mode);
static struct inode_operations assoofs_inode_ops = {
	.create = assoofs_create,
	.lookup = assoofs_lookup,
	.mkdir = assoofs_mkdir,
};

struct dentry *assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags){
	struct assoofs_inode_info *parent_info = parent_inode->i_private;
	struct super_block *sb = parent_inode->i_sb;
	struct buffer_head *bh;
	struct assoofs_dir_record_entry *record;
	int i;
	printk(KERN_INFO "Lookup request\n");
	bh = sb_bread(sb,parent_info->data_block_number);
	record = (struct assoofs_dir_record_entry *)bh->b_data;
	for(i = 0; i < parent_info->dir_children_count;i++){
		if(!strcmp(record->filename, child_dentry->d_name.name)){
			struct inode *inode = assoofs_get_inode(sb,record->inode_no);
			inode_init_owner(sb->s_user_ns,inode,parent_inode,((struct assoofs_inode_info *)inode->i_private)->mode);
			d_add(child_dentry,inode);
			return NULL;
		}
		record++;
		printk("Pruebas");
	}
	
	return NULL;
}

static int assoofs_create(struct user_namespace *mnt_userns, struct inode *dir, struct dentry *dentry, umode_t mode, bool excl){
	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct assoofs_inode_info *parent_inode_info;
	struct assoofs_dir_record_entry *dir_contents;
	struct super_block *sb;
	struct buffer_head *bh;
	uint64_t count;
	printk(KERN_INFO "New file request\n");
	
	sb = dir->i_sb;

	count = ((struct assoofs_super_block_info *)sb->s_fs_info)->inodes_count;
	inode = new_inode(sb);
	inode->i_sb = sb;
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	inode->i_op = &assoofs_inode_ops;
	inode->i_ino = count + 1;
	
	if(count > ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED -2){
		printk(KERN_ERR "The value of count is greater than the maximun of objects supported.");
		return -1;
	}
	
	inode_info = kmem_cache_alloc(assoofs_inode_cache,GFP_KERNEL);
	inode_info->inode_no = inode->i_ino;
	inode_info->file_size = 0;
	inode_info->mode = mode;
	inode->i_private = inode_info;
	
	inode->i_fop = &assoofs_file_operations;
	
	inode_init_owner(sb->s_user_ns,inode,dir,mode);
	d_add(dentry,inode);
	
	assoofs_sb_get_a_freeblock(sb,&inode_info->data_block_number);
	
	assoofs_add_inode_info(sb,inode_info);
	
	parent_inode_info = dir->i_private;
	
	bh = sb_bread(sb,parent_inode_info->data_block_number);
	
	dir_contents = (struct assoofs_dir_record_entry *)bh->b_data;
	dir_contents += parent_inode_info->dir_children_count;
	dir_contents->inode_no = inode_info->inode_no;
	
	strcpy(dir_contents->filename, dentry->d_name.name);
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	mutex_unlock(&assoofs_sb_lock);
	
	brelse(bh);
	
	parent_inode_info->dir_children_count++;
	assoofs_save_inode_info(sb,parent_inode_info);
	
	return 0;
}

static int assoofs_mkdir(struct user_namespace *mnt_userns, struct inode *dir, struct dentry *dentry, umode_t mode){
	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct assoofs_inode_info *parent_inode_info;
	struct assoofs_dir_record_entry *dir_contents;
	struct super_block *sb;
	struct buffer_head *bh;
	uint64_t count;
	printk(KERN_INFO "New directory request\n");
	sb = dir->i_sb;
	count = ((struct assoofs_super_block_info *)sb->s_fs_info)->inodes_count;
	inode = new_inode(sb);
	inode->i_sb = sb;
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	inode->i_op = &assoofs_inode_ops;
	inode->i_ino = count + 1;
	
	if(count > ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED -2){
		printk(KERN_ERR "The value of count is greater than the maximun of objects supported.");
		return -1;
	}
	
	inode_info = kmem_cache_alloc(assoofs_inode_cache,GFP_KERNEL);
	inode_info->inode_no = inode->i_ino;
	inode_info->dir_children_count = 0;
	inode_info->mode = S_IFDIR | mode;
	inode->i_private = inode_info;
	
	inode->i_fop = &assoofs_dir_operations;
	
	inode_init_owner(sb->s_user_ns,inode,dir,inode_info->mode);
	d_add(dentry,inode);
	
	assoofs_sb_get_a_freeblock(sb,&inode_info->data_block_number);
	assoofs_add_inode_info(sb,inode_info);
	
	parent_inode_info = dir->i_private;
	bh = sb_bread(sb,parent_inode_info->data_block_number);
	dir_contents = (struct assoofs_dir_record_entry *)bh->b_data;
	dir_contents += parent_inode_info->dir_children_count;
	dir_contents->inode_no = inode_info->inode_no;
	
	strcpy(dir_contents->filename, dentry->d_name.name);
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	mutex_unlock(&assoofs_sb_lock);

	
	brelse(bh);
	
	parent_inode_info->dir_children_count++;
	assoofs_save_inode_info(sb,parent_inode_info);
	
	return 0;
}

//Operaciones sobre el superbloque

static const struct super_operations assoofs_sops = {
	.destroy_inode = assoofs_destroy_inode,
};

//inicializacion del superbloque

int assoofs_fill_super(struct super_block *sb, void *data, int silent){
	struct buffer_head *bh;
	struct assoofs_super_block_info *assoofs_sb;
	struct inode *root_inode;
	printk(KERN_INFO "assoofs_fill_super request\n");
// 1.- Leer la informaci´on persistente del superbloque del dispositivo de bloque

	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	bh = sb_bread(sb, ASSOOFS_SUPERBLOCK_BLOCK_NUMBER);
	mutex_unlock(&assoofs_sb_lock);
	
	assoofs_sb = (struct assoofs_super_block_info *)bh->b_data;
	brelse(bh);
// 2.- Comprobar los parametros del superbloque
 	if(assoofs_sb->magic != ASSOOFS_MAGIC  ){
 		printk(KERN_ERR "No coinciden los numeros magicos del superbloque");
 		return -1;
 	}else if(assoofs_sb->block_size != ASSOOFS_DEFAULT_BLOCK_SIZE){
 		printk(KERN_ERR "No coinciden el tamaño de bloque del superbloque");
 		return -1;
 	}
// 3.- Escribir la informacion persistente leida del dispositivo de bloques en el superbloque sb, incluıdo el campo s_op con las operaciones que soporta.
	sb->s_magic = ASSOOFS_MAGIC;
	sb->s_maxbytes = ASSOOFS_DEFAULT_BLOCK_SIZE;
	sb->s_op = &assoofs_sops;
	sb->s_fs_info = assoofs_sb;
// 4.- Crear el inodo raız y asignarle operaciones sobre inodos (i_op) y sobre directorios (i_fop)
	root_inode = new_inode(sb);
	inode_init_owner(sb->s_user_ns, root_inode, NULL, S_IFDIR);
	root_inode->i_ino = ASSOOFS_ROOTDIR_INODE_NUMBER;
	root_inode->i_sb = sb;
	root_inode->i_op = &assoofs_inode_ops;
	root_inode->i_fop = &assoofs_dir_operations;
	root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = current_time(root_inode);
	root_inode->i_private = assoofs_get_inode_info(sb, ASSOOFS_ROOTDIR_INODE_NUMBER);
	sb->s_root = d_make_root(root_inode);
 	return 0;
}

//montaje de dispositivos assoofs

static struct dentry *assoofs_mount(struct file_system_type * fs_type, int flags, const char *dev_name, void *data){
	struct dentry *ret;
	printk(KERN_INFO "assoofs_mount request\n");
	ret = mount_bdev(fs_type, flags, dev_name, data, assoofs_fill_super);
	// Control de errores a partir del valor de ret. En este caso se puede utilizar la macro IS_ERR: if (IS_ERR(ret)) ...
	return ret;
}

//assoofs file system type

static struct file_system_type assoofs_type = {
	.owner = THIS_MODULE,
	.name = "assoofs",
	.mount = assoofs_mount,
	.kill_sb = kill_litter_super,
};

static int __init assoofs_init(void) {
	int ret;
	printk(KERN_INFO "assoofs_init request\n");
	ret = register_filesystem(&assoofs_type);
	assoofs_inode_cache = kmem_cache_create("assoofs_inode_cache", sizeof(struct assoofs_inode_info),0,(SLAB_RECLAIM_ACCOUNT|SLAB_MEM_SPREAD),NULL);
	//control de errores a partir del valor de ret
	if(ret != 0){
		printk(KERN_ERR "Error in the assoofs_init operation");
	}
	
	return ret;
}

static void __exit assoofs_exit(void){
	int ret;
	printk(KERN_INFO "assoofs_exit request\n");
	ret = unregister_filesystem(&assoofs_type);
	kmem_cache_destroy(assoofs_inode_cache);
	//control de errores a partir del valor de ret
	if(ret != 0){
		printk(KERN_ERR "Error in assoofs_exit, file system not found");
	}
}

//===========================codigo======================================
struct assoofs_inode_info *assoofs_get_inode_info(struct super_block *sb, uint64_t inode_no){
	struct assoofs_inode_info *inode_info = NULL;
	struct buffer_head *bh;
	struct assoofs_super_block_info *afs_sb = sb->s_fs_info;
	struct assoofs_inode_info *buffer = NULL;
	int i;
	printk(KERN_INFO "get inode info request");
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	bh = sb_bread(sb,ASSOOFS_INODESTORE_BLOCK_NUMBER);
	mutex_unlock(&assoofs_sb_lock);
	
	inode_info = (struct assoofs_inode_info *)bh->b_data;
	for(i = 0; i < afs_sb->inodes_count; i++){
		if(inode_info->inode_no == inode_no){
			buffer = kmem_cache_alloc(assoofs_inode_cache,GFP_KERNEL);
			memcpy(buffer,inode_info,sizeof(*buffer));
			break;
		}
		inode_info++;
	}
	brelse(bh);
	return buffer;
}

static struct inode *assoofs_get_inode(struct super_block *sb, int ino){
	struct assoofs_inode_info *inode_info;
	struct inode *inode;
	inode_info = assoofs_get_inode_info(sb,ino);
	inode = new_inode(sb);
	inode->i_ino =ino;
	inode->i_sb = sb;
	inode->i_op = &assoofs_inode_ops;
	if(S_ISDIR(inode_info->mode)){
		inode->i_fop = &assoofs_dir_operations;
	}else if(S_ISREG(inode_info->mode)){
		inode->i_fop = &assoofs_file_operations;
	}else{
		printk(KERN_ERR "Unknown inode type. Neither a directory nor a file.");
	}
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	inode->i_private = inode_info;
	return inode;
}

int assoofs_sb_get_a_freeblock(struct super_block *sb, uint64_t *block){
	struct assoofs_super_block_info *assoofs_sb = sb->s_fs_info;
	int i;
	printk(KERN_INFO "SB get a free block  request");
	for(i = 2; i < ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED; i++){
		if(assoofs_sb->free_blocks & (1 << i)){
			break;
		}	
	}
	*block = i;
	
	if(i >= ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED){
		printk(KERN_ERR "File Limit reached");
		return -1;
	}
	assoofs_sb->free_blocks &= ~(1 << i);
	assoofs_save_sb_info(sb);
	return 0;


return 0;
}

void assoofs_save_sb_info(struct super_block *vsb){
	struct buffer_head *bh;
	struct assoofs_super_block_info *sb = vsb->s_fs_info;
	printk(KERN_INFO "Save sb info request");
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	bh = sb_bread(vsb, ASSOOFS_SUPERBLOCK_BLOCK_NUMBER);
	bh-> b_data = (char *)sb;
	mutex_unlock(&assoofs_sb_lock);
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	mutex_unlock(&assoofs_sb_lock);
	
	brelse(bh);
}
void assoofs_add_inode_info(struct super_block *sb, struct assoofs_inode_info *inode){
	struct buffer_head *bh;
	struct assoofs_super_block_info *assoofs_sb = sb->s_fs_info;
    struct assoofs_inode_info *inode_info;

	printk(KERN_INFO "add inode info request");
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	bh = sb_bread(sb, ASSOOFS_INODESTORE_BLOCK_NUMBER);
	mutex_unlock(&assoofs_sb_lock);
	
	//inode = (struct assoofs_inode_info *) bh->b_data;
	//inode += assoofs_sb->inodes_count;
	//memcpy(inode, inode, sizeof(struct assoofs_inode_info));
	// AQUI ESTAS SOBREESCRIBIENDO LO QUE RECIBES COMO ARGUMENTO
    // TIENES QUE CREAR UN inode_info auxiliar
    inode_info = (struct assoofs_inode_info *)bh->b_data;
    inode_info += assoofs_sb->inodes_count;
    memcpy(inode_info, inode, sizeof(struct assoofs_inode_info));

	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	mutex_unlock(&assoofs_sb_lock);
	
	assoofs_sb->inodes_count++;
	assoofs_save_sb_info(sb);
}
int assoofs_save_inode_info(struct super_block *sb, struct assoofs_inode_info *inode_info){
	struct buffer_head *bh;
	struct assoofs_inode_info *inode_pos;
	printk(KERN_INFO "Save inode info request");
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	bh =  sb_bread(sb, ASSOOFS_INODESTORE_BLOCK_NUMBER);
	mutex_unlock(&assoofs_sb_lock);
	
	inode_pos = assoofs_search_inode_info(sb,(struct assoofs_inode_info *)bh->b_data,inode_info);
	printk(KERN_INFO "Sale del search");
	
	printk(KERN_INFO "inodo encontrado: %lld\n",inode_pos->inode_no);
	printk(KERN_INFO "inodo buscado: %lld\n",inode_info->inode_no);
	//printk(KERN_INFO "%ld",sizeof(*inode_pos));
	
	
	memcpy(inode_pos, inode_info, sizeof(*inode_pos));
	
	retM = mutex_lock_interruptible(&assoofs_sb_lock);
	if(retM != 0){
	 	printk(KERN_ERR "The mutex wasnt aplied\n");
	 }
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	mutex_unlock(&assoofs_sb_lock);
	
    brelse(bh);

	return 0;
}

struct assoofs_inode_info *assoofs_search_inode_info(struct super_block *sb, struct assoofs_inode_info *start, struct assoofs_inode_info *search){
	uint64_t count = 0;
	printk(KERN_INFO "Search inode info request");
	while(start->inode_no != search->inode_no && count < ((struct assoofs_super_block_info *)sb->s_fs_info)->inodes_count){
		count ++;
		start++;
	}
    printk(KERN_INFO "search INODO: %llu\n", start->inode_no);
	if(start->inode_no == search->inode_no){
		return start;
	}else{
		return NULL;
	}
}
//funciones de la parte opcional=========================================
void assoofs_destroy_inode(struct inode *inode){
	struct assoofs_inode *inode_info = inode->i_private;
	printk(KERN_INFO "Freing private data of inode %p (%lu)\n",inode_info,inode->i_ino);
	kmem_cache_free(assoofs_inode_cache,inode_info);
}

//=======================================================================

module_init(assoofs_init);
module_exit(assoofs_exit);
