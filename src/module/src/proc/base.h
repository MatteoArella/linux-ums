/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_BASE_H
#define PROC_BASE_H

#include <linux/proc_fs.h>

#define PROC_PID_BUFLEN				12

/**
 * UMS procfs base directories for each process that opens the UMS device
 */
struct ums_proc_dirs {
	/**
	 * procfs base directory path, i.e. `/proc/ums/<pid>`
	 */
	char *pid_dir_path;

	/**
	 * procfs base directory path length
	 */
	size_t pid_dir_path_size;

	/**
	 * procfs base directory
	 */
	struct proc_dir_entry *pid_dir;

	/**
	 * procfs schedulers directory located at `/proc/ums/<pid>/schedulers`
	 */
	struct proc_dir_entry *sched_dir;

	/**
	 * procfs workers directory path, i.e. `/proc/ums/<pid>/workers`
	 */
	char *workers_dir_path;

	/**
	 * procfs workers directory path length
	 */
	size_t workers_dir_path_size;

	/**
	 * procfs workers directory
	 */
	struct proc_dir_entry *workers_dir;
};

/**
 * @brief Initialize the UMS procfs
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 */
int ums_proc_init(void);

/**
 * @brief Destroy the UMS procfs
 */
void ums_proc_destroy(void);

/**
 * @brief Initialize the UMS procfs base directories
 *
 * @param[in] dirs		pointer to the UMS procfs directories
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 */
int ums_proc_dirs_init(struct ums_proc_dirs *dirs);

/**
 * @brief Destroy the UMS procfs base directories
 *
 * @param[in] dirs		pointer to the UMS procfs directories
 */
void ums_proc_dirs_destroy(struct ums_proc_dirs *dirs);

#endif /* PROC_BASE_H */
