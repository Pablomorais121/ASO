#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pablo Morais Alvarez");

static int __int init_hello(void)
{
	print(KERN_INFO "Hello world\n");
	return 0;
}

static void __exit cleanup_hello(void)
{
	printf(KERN_INFO"Goodby world\n");
}

module_init(init_hello);
module_exit(cleanup_hello);
