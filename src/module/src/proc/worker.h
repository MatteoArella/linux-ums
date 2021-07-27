/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_WORKER_H
#define PROC_WORKER_H

#include "base.h"

struct ums_worker;

struct ums_worker_complist_node {
	struct proc_dir_entry *worker_link;
};

struct ums_worker_proc_dirs {
	char *worker_dir_path;
	size_t worker_dir_path_size;
	struct proc_dir_entry *worker_dir;
	struct proc_dir_entry *worker_info_dir;
	struct ums_worker_complist_node complist_node;
};

int ums_worker_proc_register(struct ums_proc_dirs *dirs,
			     struct ums_worker *worker);

void ums_worker_proc_unregister(struct ums_worker *worker);

#endif /* PROC_WORKER_H */
