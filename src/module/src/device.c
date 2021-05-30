#include "device.h"

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>

static int ums_dev_open(struct inode *inode, struct file *filp)
{
	/* check that device is opened in read-only mode */
	if ((filp->f_flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

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
