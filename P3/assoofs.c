#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include "assoofs.h"

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
	printK(KERN_INFO "Write request\n");
	return 0;
}

//Operaciones sobre directorios

static in assoofs_iterate(struct file *filp, struct dir_context *ctx);
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
	printk(KERN_INFO "assoofs_fill_super request\n");
	// 1.- Leer la informaci´on persistente del superbloque del dispositivo de bloques
 	// 2.- Comprobar los par´ametros del superbloque
 	// 3.- Escribir la informaci´on persistente le´ıda del dispositivo de bloques en el 		       superbloque sb, inclu´ıdo el campo s_op con las operaciones que soporta.
 	// 4.- Crear el inodo ra´ız y asignarle operaciones sobre inodos (i_op) y sobre 	       directorios (i_fop)
 	return 0;
}

//montaje de dispositivos assoofs

static struct dentry *assoofs_mount(struct file_system_type * fs_type, int flags, const char *dev_name, void *data){
	struct dentry *ret;
	printk(KERN_INFO "assoofs_mount request\n");
	ret = mount_bdev(fs_type, flags, dev_name, data, assoofs_fill_super)
}

//assoofs file system type

static struct file_system_type assoofs_type = {
	.owner = THIS_MODULE,
	.name = "assoofs",
	.mount = assoofs_mount,
	.kill_sb = kill_litter_super,
};

static __init assoofs_init(void) {
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

module_init(assoofs_init);
module_exit(assoofs_exit);
