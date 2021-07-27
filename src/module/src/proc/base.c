// SPDX-License-Identifier: AGPL-3.0-only

#include "../log.h"
#include "base.h"

#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#define UMS_PROC_BASEDIR_NAME		"ums"
#define UMS_PROC_BASEDIR_PATH		"/proc/" UMS_PROC_BASEDIR_NAME
#define UMS_PROC_SCHED_DIR_NAME		"schedulers"
#define UMS_PROC_WORKERS_DIR_NAME	"workers"

static struct proc_dir_entry *ums_proc_base_dir;
static size_t ums_proc_base_dir_path_len;

int ums_proc_init(void)
{
	ums_proc_base_dir_path_len = strlen(UMS_PROC_BASEDIR_PATH);
	ums_proc_base_dir = proc_mkdir(UMS_PROC_BASEDIR_NAME, NULL);
	return ums_proc_base_dir != NULL ? 0 : -ENOMEM;
}

void ums_proc_destroy(void)
{
	proc_remove(ums_proc_base_dir);
}

int ums_proc_dirs_init(struct ums_proc_dirs *dirs)
{
	char proc_pid_name[PROC_PID_BUFLEN];
	size_t proc_pid_name_len;
	int retval;
	pid_t proc_pid;

	proc_pid = task_pid_vnr(current);
	retval = snprintf(proc_pid_name, PROC_PID_BUFLEN, "%d", proc_pid);
	if (unlikely(!retval))
		return -ENOMEM;

	proc_pid_name_len = strlen(proc_pid_name);
	dirs->pid_dir = proc_mkdir(proc_pid_name, ums_proc_base_dir);
	if (unlikely(!dirs->pid_dir))
		return -ENOMEM;

	dirs->pid_dir_path_size = ums_proc_base_dir_path_len +
				  proc_pid_name_len + 1;
	dirs->pid_dir_path = kmalloc(dirs->pid_dir_path_size + 1, GFP_KERNEL);
	if (unlikely(!dirs->pid_dir_path)) {
		retval = -ENOMEM;
		goto proc_pid_dir;
	}

	strcpy(dirs->pid_dir_path, UMS_PROC_BASEDIR_PATH);
	strcat(dirs->pid_dir_path, "/");
	strcat(dirs->pid_dir_path, proc_pid_name);

	dirs->sched_dir = proc_mkdir(UMS_PROC_SCHED_DIR_NAME, dirs->pid_dir);
	if (unlikely(!dirs->sched_dir)) {
		retval = -ENOMEM;
		goto proc_pid_path;
	}

	dirs->workers_dir = proc_mkdir(UMS_PROC_WORKERS_DIR_NAME,
				       dirs->pid_dir);
	if (unlikely(!dirs->workers_dir)) {
		retval = -ENOMEM;
		goto proc_sched_dir;
	}

	dirs->workers_dir_path_size = dirs->pid_dir_path_size +
				      strlen(UMS_PROC_WORKERS_DIR_NAME) + 1;
	dirs->workers_dir_path = kmalloc(dirs->workers_dir_path_size + 1,
					 GFP_KERNEL);
	if (unlikely(!dirs->workers_dir_path)) {
		retval = -ENOMEM;
		goto proc_workers_dir;
	}

	strcpy(dirs->workers_dir_path, dirs->pid_dir_path);
	strcat(dirs->workers_dir_path, "/");
	strcat(dirs->workers_dir_path, UMS_PROC_WORKERS_DIR_NAME);

	return 0;

proc_workers_dir:
	proc_remove(dirs->workers_dir);
proc_sched_dir:
	proc_remove(dirs->sched_dir);
proc_pid_path:
	kfree(dirs->pid_dir_path);
proc_pid_dir:
	proc_remove(dirs->pid_dir);
	return retval;
}

void ums_proc_dirs_destroy(struct ums_proc_dirs *dirs)
{
	proc_remove(dirs->pid_dir);
	proc_remove(dirs->sched_dir);
	proc_remove(dirs->workers_dir);
	kfree(dirs->pid_dir_path);
	kfree(dirs->workers_dir_path);
}
