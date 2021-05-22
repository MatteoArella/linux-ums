#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init ums_init(void)
{
	return 0;
}

static void __init ums_exit(void)
{

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matteo Arella <arella.matteo.95@gmail.com>");
MODULE_DESCRIPTION("UMS module");
MODULE_VERSION("1.0.0");

module_init(ums_init);
module_exit(ums_exit);
