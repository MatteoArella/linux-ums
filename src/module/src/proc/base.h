/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_BASE_H
#define PROC_BASE_H

#include <linux/proc_fs.h>

#define PROC_PID_BUFLEN				12

struct ums_proc_dirs {
	char *pid_dir_path;
	size_t pid_dir_path_size;
	struct proc_dir_entry *pid_dir;
	struct proc_dir_entry *sched_dir;
	char *workers_dir_path;
	size_t workers_dir_path_size;
	struct proc_dir_entry *workers_dir;
};

int ums_proc_init(void);
void ums_proc_destroy(void);

int ums_proc_dirs_init(struct ums_proc_dirs *dirs);
void ums_proc_dirs_destroy(struct ums_proc_dirs *dirs);

#endif /* PROC_BASE_H */
