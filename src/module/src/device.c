// SPDX-License-Identifier: AGPL-3.0-only

#include "ums.h"
#include "device.h"
#include "complist.h"
#include "scheduler.h"
#include "worker.h"

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/errno.h>

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

	/* check that device is opened in read-only mode */
	if ((filp->f_flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	ums_data = kzalloc(sizeof(struct ums_data), GFP_KERNEL);

	if (unlikely(!ums_data))
		return -ENOMEM;

	IDR_L_INIT(&ums_data->comp_lists);
	IDR_L_INIT(&ums_data->schedulers);
	IDR_L_INIT(&ums_data->workers);

	filp->private_data = ums_data;

	return 0;
}

static int scheduler_destroy(int id, void *sched, void *data)
{
	return ums_scheduler_destroy(sched);
}

static int worker_destroy(int id, void *worker, void *data)
{
	return ums_worker_destroy(worker);
}

static int complist_destroy(int id, void *complist, void *data)
{
	return ums_complist_destroy(complist);
}

static int ums_dev_release(struct inode *inode, struct file *filp)
{
	struct ums_data *ums_data;

	ums_data = filp->private_data;

	// destroy all
	IDR_L_FOR_EACH(&ums_data->schedulers, scheduler_destroy, NULL);
	IDR_L_FOR_EACH(&ums_data->workers, worker_destroy, NULL);
	IDR_L_FOR_EACH(&ums_data->comp_lists, complist_destroy, NULL);

	IDR_L_DESTROY(&ums_data->comp_lists);
	IDR_L_DESTROY(&ums_data->schedulers);
	IDR_L_DESTROY(&ums_data->workers);

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
				(struct ums_sched_dqevent_args __user *) arg);
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
	case IOCTL_UMS_YIELD:
		retval = -ENOTSUPP;
		break;
	default:
		retval = -ENOTTY;
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
