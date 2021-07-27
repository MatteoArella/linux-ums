// SPDX-License-Identifier: AGPL-3.0-only

#include "../log.h"
#include "context.h"
#include "worker.h"
#include "../worker.h"

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define UMS_PROC_WORKER_INFO_BUFLEN		512
#define UMS_PROC_WORKER_INFO_NAME		"info"

static ssize_t proc_read_worker_info(struct file *filp, char __user *ubuf,
				     size_t count, loff_t *ppos);

#if KERNEL_VERSION(5, 6, 0) <= LINUX_VERSION_CODE
static const struct proc_ops proc_fops = {
	.proc_read = proc_read_worker_info
};
#else
static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.read = proc_read_worker_info
};
#endif

int ums_worker_proc_register(struct ums_proc_dirs *dirs,
			     struct ums_worker *worker)
{
	char worker_pid_name[PROC_PID_BUFLEN];
	size_t worker_pid_name_len;
	int retval;

	retval = snprintf(worker_pid_name,
			  PROC_PID_BUFLEN,
			  "%d",
			  worker->context.pid);
	if (unlikely(!retval))
		return -ENOMEM;
	worker_pid_name_len = strlen(worker_pid_name);

	worker->dirs.worker_dir = proc_mkdir(worker_pid_name,
					     dirs->workers_dir);
	if (unlikely(!worker->dirs.worker_dir))
		return -ENOMEM;

	worker->dirs.worker_dir_path_size = dirs->workers_dir_path_size +
					    worker_pid_name_len + 1;
	worker->dirs.worker_dir_path =
		kmalloc(worker->dirs.worker_dir_path_size + 1, GFP_KERNEL);
	if (unlikely(!worker->dirs.worker_dir_path)) {
		retval = -ENOMEM;
		goto worker_dir_path;
	}
	strcpy(worker->dirs.worker_dir_path, dirs->workers_dir_path);
	strcat(worker->dirs.worker_dir_path, "/");
	strcat(worker->dirs.worker_dir_path, worker_pid_name);

	worker->dirs.worker_info_dir = proc_create_data(
						UMS_PROC_WORKER_INFO_NAME,
						S_IALLUGO,
						worker->dirs.worker_dir,
						&proc_fops,
						worker);
	if (unlikely(!worker->dirs.worker_info_dir)) {
		retval = -ENOMEM;
		goto worker_info_dir;
	}

	return 0;

worker_info_dir:
	kfree(worker->dirs.worker_dir_path);
worker_dir_path:
	proc_remove(worker->dirs.worker_dir);
	return retval;
}

void ums_worker_proc_unregister(struct ums_worker *worker)
{
	proc_remove(worker->dirs.worker_info_dir);
	proc_remove(worker->dirs.worker_dir);
	kfree(worker->dirs.worker_dir_path);
}

static ssize_t proc_read_worker_info(struct file *filp, char __user *ubuf,
				     size_t count, loff_t *ppos)
{
	char worker_info_buf[UMS_PROC_WORKER_INFO_BUFLEN];
	struct ums_worker *worker;
	int retval;
	ssize_t len = 0;
	loff_t pos = *ppos;

	if (pos < 0)
		return -EINVAL;

	worker = PDE_DATA(filp->f_inode);

	len = context_snprintf(worker_info_buf, UMS_PROC_WORKER_INFO_BUFLEN,
			       &worker->context);

	if (pos >= len || !count)
		return 0;

	if (pos + count > len)
		count = len - pos;

	retval = copy_to_user(ubuf, worker_info_buf + pos, count);
	if (retval == count)
		return -EACCES;

	count -= retval;
	*ppos += count;
	return count;
}
