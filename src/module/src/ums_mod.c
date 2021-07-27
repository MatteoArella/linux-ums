// SPDX-License-Identifier: AGPL-3.0-only

#include "log.h"
#include "ums.h"
#include "device.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init ums_init(void)
{
	int retval;

	retval = ums_caches_init();
	if (retval)
		goto cache_init;

	retval = register_ums_device();
	if (retval)
		goto register_dev;

	retval = ums_proc_init();
	if (retval)
		goto proc_init;

	return 0;

proc_init:
	unregister_ums_device();
register_dev:
	ums_caches_destroy();
cache_init:
	return retval;
}

static void __exit ums_exit(void)
{
	unregister_ums_device();
	ums_caches_destroy();
	ums_proc_destroy();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matteo Arella <arella.matteo.95@gmail.com>");
MODULE_DESCRIPTION("UMS module");
MODULE_VERSION("1.0.0");

module_init(ums_init);
module_exit(ums_exit);
