#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

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

#ifdef CONFIG_MITIGATION_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x5f510b6b, "find_get_pid" },
	{ 0x13404d69, "pid_task" },
	{ 0x9688de8b, "memstart_addr" },
	{ 0x122c3a7e, "_printk" },
	{ 0xfe0441ca, "param_ops_ullong" },
	{ 0x3efc6d60, "param_ops_int" },
	{ 0x7d439289, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "1CC473754FC1837AFD6A0C8");
