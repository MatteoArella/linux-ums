// SPDX-License-Identifier: AGPL-3.0-only

#include "context.h"
#include "scheduler.h"
#include "../scheduler.h"
#include "../worker.h"

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define UMS_PROC_SCHED_INFO_BUFLEN		512
#define UMS_PROC_SCHED_SYMLINK_BUFLEN		12
#define UMS_PROC_SCHED_INFO_NAME		"info"
#define UMS_PROC_SCHED_WORKERS_NAME		"workers"

static ssize_t proc_read_scheduler_info(struct file *filp, char __user *ubuf,
					size_t count, loff_t *ppos);

#if KERNEL_VERSION(5, 6, 0) <= LINUX_VERSION_CODE
static const struct proc_ops proc_fops = {
	.proc_read = proc_read_scheduler_info
};
#else
static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.read = proc_read_scheduler_info
};
#endif

int ums_scheduler_proc_register(struct ums_proc_dirs *dirs,
				struct ums_scheduler *scheduler)
{
	char sched_pid_name[PROC_PID_BUFLEN];
	int retval;

	retval = snprintf(sched_pid_name,
			  PROC_PID_BUFLEN,
			  "%d",
			  scheduler->context.pid);
	if (unlikely(!retval))
		return -ENOMEM;
	scheduler->dirs.scheduler_dir = proc_mkdir(sched_pid_name,
						   dirs->sched_dir);
	if (unlikely(!scheduler->dirs.scheduler_dir))
		return -ENOMEM;

	scheduler->dirs.scheduler_info_dir =
			proc_create_data(UMS_PROC_SCHED_INFO_NAME,
					 S_IALLUGO,
					 scheduler->dirs.scheduler_dir,
					 &proc_fops,
					 scheduler);
	if (unlikely(!scheduler->dirs.scheduler_info_dir)) {
		retval = -ENOMEM;
		goto out;
	}

	scheduler->dirs.workers_dir =
			proc_mkdir(UMS_PROC_SCHED_WORKERS_NAME,
				   scheduler->dirs.scheduler_dir);
	if (unlikely(!scheduler->dirs.workers_dir)) {
		retval = -ENOMEM;
		goto sched_workers_proc;
	}

	scheduler->dirs.n_workers = scheduler->dirs.max_workers = 0L;

	return 0;

sched_workers_proc:
	proc_remove(scheduler->dirs.scheduler_info_dir);
out:
	proc_remove(scheduler->dirs.scheduler_dir);
	return retval;
}

void ums_scheduler_proc_unregister(struct ums_scheduler *scheduler)
{
	proc_remove(scheduler->dirs.workers_dir);
	proc_remove(scheduler->dirs.scheduler_info_dir);
	proc_remove(scheduler->dirs.scheduler_dir);
}

static ssize_t proc_read_scheduler_info(struct file *filp, char __user *ubuf,
					size_t count, loff_t *ppos)
{
	char sched_info_buf[UMS_PROC_SCHED_INFO_BUFLEN];
	struct ums_scheduler *scheduler;
	int retval;
	ssize_t len = 0;
	loff_t pos = *ppos;

	if (pos < 0)
		return -EINVAL;

	scheduler = PDE_DATA(filp->f_inode);

	len = context_snprintf(sched_info_buf, UMS_PROC_SCHED_INFO_BUFLEN,
			       &scheduler->context);

	if (pos >= len || !count)
		return 0;

	if (pos + count > len)
		count = len - pos;

	retval = copy_to_user(ubuf, sched_info_buf + pos, count);
	if (retval == count)
		return -EACCES;

	count -= retval;
	*ppos += count;
	return count;
}

int ums_scheduler_proc_register_worker(struct ums_scheduler_proc_dirs *dirs,
				struct ums_worker_proc_dirs *worker_dirs)
{
	char symlink_name[UMS_PROC_SCHED_SYMLINK_BUFLEN];
	int retval;

	retval = snprintf(symlink_name, UMS_PROC_SCHED_SYMLINK_BUFLEN, "%lu",
			  dirs->max_workers + 1);
	if (unlikely(!retval))
		return -ENOMEM;

	worker_dirs->complist_node.worker_link =
				proc_symlink(symlink_name,
					     dirs->workers_dir,
					     worker_dirs->worker_dir_path);
	if (unlikely(!worker_dirs->complist_node.worker_link))
		return -ENOMEM;

	dirs->n_workers++;
	dirs->max_workers++;

	return 0;
}

void ums_scheduler_proc_unregister_worker(struct ums_scheduler_proc_dirs *dirs,
				struct ums_worker_proc_dirs *worker_dirs)
{
	proc_remove(worker_dirs->complist_node.worker_link);

	dirs->n_workers--;
	if (!dirs->n_workers)
		dirs->max_workers = 0L;
}
