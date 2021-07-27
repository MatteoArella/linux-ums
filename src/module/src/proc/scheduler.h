/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_SCHEDULER_H
#define PROC_SCHEDULER_H

#include "base.h"
#include "worker.h"

struct ums_scheduler;

struct ums_scheduler_proc_dirs {
	struct proc_dir_entry *scheduler_dir;
	struct proc_dir_entry *scheduler_info_dir;
	struct proc_dir_entry *workers_dir;

	unsigned long n_workers;
	unsigned long max_workers;
};

int ums_scheduler_proc_register(struct ums_proc_dirs *dirs,
				struct ums_scheduler *scheduler);

void ums_scheduler_proc_unregister(struct ums_scheduler *scheduler);

int ums_scheduler_proc_register_worker(struct ums_scheduler_proc_dirs *dirs,
				struct ums_worker_proc_dirs *worker_dirs);

void ums_scheduler_proc_unregister_worker(struct ums_scheduler_proc_dirs *dirs,
				struct ums_worker_proc_dirs *worker_dirs);

#endif /* PROC_SCHEDULER_H */
