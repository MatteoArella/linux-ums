#include "ums.h"
#include "device.h"
#include "scheduler.h"

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

static int ums_dev_release(struct inode *inode, struct file *filp)
{
	struct ums_data *ums_data;

	ums_data = filp->private_data;

	// destroy all
	IDR_L_FOR_EACH(&ums_data->schedulers, scheduler_destroy, NULL);

	IDR_L_DESTROY(&ums_data->comp_lists);
	IDR_L_DESTROY(&ums_data->schedulers);
	IDR_L_DESTROY(&ums_data->workers);

	kfree(filp->private_data);
	filp->private_data = NULL;

	return 0;
}

#ifdef HAVE_UNLOCKED_IOCTL
static long ums_dev_ioctl(struct file *filp,
			  unsigned int cmd,
			  unsigned long arg)
{
	long retval;

	switch (cmd) {
	case IOCTL_ENTER_UMS:
		retval = enter_ums_mode((struct ums_data *) filp->private_data,
				(struct enter_ums_mode_args __user *) arg);
		break;
	case IOCTL_UMS_SCHED_DQEVENT:
		retval = ums_sched_dqevent(
				(struct ums_data *) filp->private_data,
				(struct ums_sched_dqevent_args __user *) arg);
		break;
	case IOCTL_CREATE_UMS_CTX:
	case IOCTL_CREATE_UMS_CLIST:
	case IOCTL_DEQUEUE_UMS_CLIST:
	case IOCTL_NEXT_UMS_CLIST:
	case IOCTL_EXEC_UMS_CTX:
	case IOCTL_UMS_YIELD:
	case IOCTL_DELETE_UMS_CLIST:
	case IOCTL_DELETE_UMS_CTX:
		retval = -ENOSYS;
		break;
	default:
		retval = -ENOTTY;
	}
	return retval;
}
#else
static int ums_dev_ioctl(struct inode *node,
			 struct file *filp,
			 unsigned int cmd,
			 unsigned long arg)
{
	int retval;

	switch (cmd) {
	case IOCTL_ENTER_UMS:
		retval = enter_ums_mode((struct ums_data *) filp->private_data,
				(struct enter_ums_mode_args __user *) arg);
		break;
	case IOCTL_UMS_SCHED_DQEVENT:
		retval = ums_sched_dqevent(
				(struct ums_data *) filp->private_data,
				(struct ums_sched_dqevent_args __user *) arg);
		break;
	case IOCTL_CREATE_UMS_CTX:
	case IOCTL_CREATE_UMS_CLIST:
	case IOCTL_DEQUEUE_UMS_CLIST:
	case IOCTL_NEXT_UMS_CLIST:
	case IOCTL_EXEC_UMS_CTX:
	case IOCTL_UMS_YIELD:
	case IOCTL_DELETE_UMS_CLIST:
	case IOCTL_DELETE_UMS_CTX:
		retval = -ENOSYS;
		break;
	default:
		retval = -ENOTTY;
	}
	return retval;
}
#endif

static struct file_operations ums_fops = {
	.owner = THIS_MODULE,
	.open = ums_dev_open,
	.release = ums_dev_release,

#ifdef HAVE_UNLOCKED_IOCTL
	.unlocked_ioctl = ums_dev_ioctl,

#ifdef HAVE_COMPAT_IOCTL
	.compat_ioctl = ums_dev_ioctl
#endif

#else
	.ioctl = ums_dev_ioctl,
#endif
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
