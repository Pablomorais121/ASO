#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include "assoofs.h"


//prototipos que he escrito yo===========================================
extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);
static struct dentry *assoofs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data);
extern struct dentry *mount_bdev(struct file_system_type *fs_type, int flags, const char *dev_name, void *data, int (*fill_super)(struct super_block *, void *, int));
int assoofs_fill_super(struct super_block *sb, void *data, int silent);
extern struct inode *new_inode(struct super_block *sb);
struct assoofs_inode_info *assoofs_get_inode_info(struct super_block *sb, uint64_t inode_no);
//=======================================================================
//Operaciones sobre ficheros

ssize_t assoofs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos);
ssize_t assoofs_write(struct file *filp, const char __user * buf, size_t len, loff_t *ppos);
const struct file_operations assoofs_file_operations = {
	.read = assoofs_read,
	.write = assoofs_write,
};

ssize_t assoofs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos){
	printk(KERN_INFO "Read request\n");
	return 0;
}

ssize_t assoofs_write(struct file *filp, const char __user *buff, size_t len, loff_t *ppos){
	printk(KERN_INFO "Write request\n");
	return 0;
}

//Operaciones sobre directorios

static int assoofs_iterate(struct file *filp, struct dir_context *ctx);
const struct file_operations assoofs_dir_operations = {
	.owner = THIS_MODULE,
	.iterate = assoofs_iterate,
};

static int assoofs_iterate(struct file *filp, struct dir_context *ctx){
	printk(KERN_INFO "Iterate request\n");
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
	printk(KERN_INFO "Lookup request\n");
	return 0;
}

static int assoofs_create(struct user_namespace *mnt_userns, struct inode *dir, struct dentry *dentry, umode_t mode, bool excl){
	printk(KERN_INFO "New file request\n");
	return 0;
}

static int assoofs_mkdir(struct user_namespace *mnt_userns, struct inode *dir, struct dentry *dentry, umode_t mode){
	printk(KERN_INFO "New directory request\n");
	return 0;
}

//Operaciones sobre el superbloque

static const struct super_operations assoofs_sops = {
	.drop_inode = generic_delete_inode,
};

//inicializacion del superbloque

int assoofs_fill_super(struct super_block *sb, void *data, int silent){
	struct buffer_head *bh;
	struct assoofs_super_block_info *assoofs_sb;
	struct inode *root_inode;
	printk(KERN_INFO "assoofs_fill_super request\n");
// 1.- Leer la informaci´on persistente del superbloque del dispositivo de bloque
	bh = sb_bread(sb, ASSOOFS_SUPERBLOCK_BLOCK_NUMBER);
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
	//control de errores a partir del valor de ret
	return ret;
}

static void __exit assoofs_exit(void){
	int ret;
	printk(KERN_INFO "assoofs_exit request\n");
	ret = unregister_filesystem(&assoofs_type);
	//control de errores a partir del valor de ret
}

//===========================codigo======================================
//=======================================================================

module_init(assoofs_init);
module_exit(assoofs_exit);
