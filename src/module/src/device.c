// SPDX-License-Identifier: AGPL-3.0-only

#include "ums.h"
#include "device.h"
#include "complist.h"
#include "scheduler.h"
#include "worker.h"
#include "proc/base.h"

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/rhashtable.h>

inline int ums_caches_init(void)
{
	return ums_scheduling_cache_create();
}

inline void ums_caches_destroy(void)
{
	ums_scheduling_cache_destroy();
}

static int ums_dev_open(struct inode *inode, struct file *filp)
{
	struct ums_data *ums_data;
	int retval;

	/* check that device is opened in read-only mode */
	if ((filp->f_flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	ums_data = kzalloc(sizeof(struct ums_data), GFP_KERNEL);
	if (unlikely(!ums_data))
		return -ENOMEM;

	/* initialize proc dirs */
	retval = ums_proc_dirs_init(&ums_data->dirs);
	if (unlikely(retval))
		goto proc_dirs_init;

	IDR_L_INIT(&ums_data->comp_lists);

	retval = rhashtable_init(&ums_data->context_table,
				 &ums_context_params);
	if (unlikely(retval))
		goto rhashtable_init;

	filp->private_data = ums_data;

	return 0;

rhashtable_init:
	ums_proc_dirs_destroy(&ums_data->dirs);
proc_dirs_init:
	kfree(ums_data);
	return retval;
}

static void release_context(void *ptr, void *arg)
{
	struct ums_context *context = ptr;

	context->release(context);
}

static int release_complist(int id, void *complist, void *data)
{
	put_ums_complist(complist);
	return 0;
}

static int ums_dev_release(struct inode *inode, struct file *filp)
{
	struct ums_data *ums_data;

	ums_data = filp->private_data;

	// destroy all
	rhashtable_free_and_destroy(&ums_data->context_table,
				    release_context,
				    NULL);

	IDR_L_FOR_EACH(&ums_data->comp_lists, release_complist, NULL);
	IDR_L_DESTROY(&ums_data->comp_lists);

	ums_proc_dirs_destroy(&ums_data->dirs);

	kfree(filp->private_data);
	filp->private_data = NULL;

	return 0;
}

static long ums_dev_ioctl(struct file *filp,
			  unsigned int cmd,
			  unsigned long arg)
{
	long retval;

	switch (cmd) {
	case IOCTL_CREATE_UMS_CLIST:
		retval = create_ums_completion_list(
				(struct ums_data *) filp->private_data,
				(ums_comp_list_id_t __user *) arg);
		break;
	case IOCTL_ENTER_UMS:
		retval = enter_ums_mode((struct ums_data *) filp->private_data,
				(struct enter_ums_mode_args __user *) arg);
		break;
	case IOCTL_UMS_SCHED_DQEVENT:
		retval = ums_sched_dqevent(
				(struct ums_data *) filp->private_data,
				(struct ums_sched_event __user *) arg);
		break;
	case IOCTL_DEQUEUE_UMS_CLIST:
		retval = ums_complist_dqcontext(
			(struct ums_data *) filp->private_data,
			(struct dequeue_ums_complist_args __user *) arg);
		break;
	case IOCTL_NEXT_UMS_CTX_LIST:
		retval = ums_complist_next_context(
			(struct ums_data *) filp->private_data,
			(struct ums_next_context_list_args __user *) arg);
		break;
	case IOCTL_DELETE_UMS_CLIST:
		retval = ums_complist_delete(
			(struct ums_data *) filp->private_data, arg);
		break;
	case IOCTL_EXEC_UMS_CTX:
		retval = exec_ums_context(
			(struct ums_data *) filp->private_data, arg);
		break;
	case IOCTL_UMS_YIELD:
		retval = ums_thread_yield(
			(struct ums_data *) filp->private_data,
			(void __user *) arg);
		break;
	case IOCTL_EXIT_UMS:
		retval = ums_thread_end(
			(struct ums_data *) filp->private_data);
		break;
	default:
		retval = -ENOTSUPP;
	}
	return retval;
}

static const struct file_operations ums_fops = {
	.owner = THIS_MODULE,
	.open = ums_dev_open,
	.release = ums_dev_release,
	.unlocked_ioctl = ums_dev_ioctl,
	.compat_ioctl = ums_dev_ioctl
};

static struct miscdevice ums_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = UMS_DEV_NAME,
	.fops = &ums_fops,
	.mode = S_IALLUGO
};

inline int register_ums_device(void)
{
	return misc_register(&ums_dev);
}

inline void unregister_ums_device(void)
{
	misc_deregister(&ums_dev);
}
