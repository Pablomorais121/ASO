#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd9726f80, "module_layout" },
	{ 0xd63ee8ba, "generic_delete_inode" },
	{ 0xe16ade6b, "kill_litter_super" },
	{ 0x1f290272, "unregister_filesystem" },
	{ 0x741bf5fb, "register_filesystem" },
	{ 0x2db6c90e, "mount_bdev" },
	{ 0xe9bccd4a, "d_make_root" },
	{ 0xa6a1a47d, "current_time" },
	{ 0xd879ad2a, "inode_init_owner" },
	{ 0x5dfb552f, "new_inode" },
	{ 0xc6b9d59e, "__brelse" },
	{ 0x4a9b759b, "__bread_gfp" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B0F2D66CB66E73B244C7BCF");
