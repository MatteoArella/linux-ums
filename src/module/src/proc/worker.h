/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_WORKER_H
#define PROC_WORKER_H

#include "base.h"

struct ums_worker;

/**
 * UMS completion list node with UMS worker
 */
struct ums_worker_complist_node {
	/**
	 * Symbolic link targeting the UMS worker
	 */
	struct proc_dir_entry *worker_link;
};

/**
 * UMS worker procfs directories
 */
struct ums_worker_proc_dirs {
	/**
	 * procfs worker directory path, i.e.
	 * `/proc/ums/<pid>/workers/<worker-pid>`
	 */
	char *worker_dir_path;

	/**
	 * procfs worker directory path length
	 */
	size_t worker_dir_path_size;

	/**
	 * procfs worker directory
	 */
	struct proc_dir_entry *worker_dir;

	/**
	 * worker info file located at
	 * `/proc/ums/<pid>/workers/<worker-pid>/info`
	 */
	struct proc_dir_entry *worker_info_dir;

	/**
	 * procfs worker node with a completion list
	 */
	struct ums_worker_complist_node complist_node;
};

/**
 * @brief Register the UMS worker to the UMS procfs
 *
 * @param[in,out] dirs		pointer to the UMS procfs directories
 * @param[in] worker		pointer to the UMS worker
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 */
int ums_worker_proc_register(struct ums_proc_dirs *dirs,
			     struct ums_worker *worker);

/**
 * @brief Unregister the UMS worker to the UMS procfs
 *
 * @param[in] worker		pointer to the UMS worker
 */
void ums_worker_proc_unregister(struct ums_worker *worker);

#endif /* PROC_WORKER_H */
