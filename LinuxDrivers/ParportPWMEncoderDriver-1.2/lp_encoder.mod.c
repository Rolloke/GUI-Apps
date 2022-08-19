#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xbc257400, "module_layout" },
	{ 0x72aa82c6, "param_ops_charp" },
	{ 0xf59f197, "param_array_ops" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0x42160169, "flush_workqueue" },
	{ 0xd5f2172f, "del_timer_sync" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x88a193, "__alloc_workqueue_key" },
	{ 0xb1e5948, "__register_chrdev" },
	{ 0x48eb0c0d, "__init_waitqueue_head" },
	{ 0x75bb675a, "finish_wait" },
	{ 0x622fa02a, "prepare_to_wait" },
	{ 0x4292364c, "schedule" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xf296a0d6, "current_task" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x62cd8754, "__tracepoint_module_get" },
	{ 0x2a6c3c71, "module_put" },
	{ 0x47c149ab, "queue_delayed_work" },
	{ 0xe45f60d8, "__wake_up" },
	{ 0x91715312, "sprintf" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "CB50ACE44DD1199FE5DA5FA");
