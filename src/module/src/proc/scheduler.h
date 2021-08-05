/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_SCHEDULER_H
#define PROC_SCHEDULER_H

#include "base.h"
#include "worker.h"

struct ums_scheduler;

/**
 * UMS scheduler procfs directories
 */
struct ums_scheduler_proc_dirs {
	/**
	 * procfs scheduler directory located at
	 * `/proc/ums/<pid>/schedulers/<sched-pid>`
	 */
	struct proc_dir_entry *scheduler_dir;

	/**
	 * scheduler info file located at
	 * `/proc/ums/<pid>/schedulers/<sched-pid>/info`
	 */
	struct proc_dir_entry *scheduler_info_dir;

	/**
	 * procfs scheduler's workers directory located at
	 * `/proc/ums/<pid>/schedulers/<sched-pid>/workers`
	 */
	struct proc_dir_entry *workers_dir;

	/**
	 * number of UMS workers dequeued from the UMS scheduler's completion
	 * list
	 */
	unsigned long n_workers;
	unsigned long max_workers;
};

/**
 * @brief Register the UMS scheduler to the UMS procfs
 *
 * @param[in,out] dirs		pointer to the UMS procfs directories
 * @param[in] scheduler		pointer to the UMS scheduler
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 */
int ums_scheduler_proc_register(struct ums_proc_dirs *dirs,
				struct ums_scheduler *scheduler);

/**
 * @brief Unregister the UMS scheduler to the UMS procfs
 *
 * @param[in] scheduler		pointer to the UMS scheduler
 */
void ums_scheduler_proc_unregister(struct ums_scheduler *scheduler);

/**
 * @brief Register the UMS worker to the UMS scheduler procfs
 *
 * Create a symlink inside `/proc/ums/<pid>/schedulers/<sched-pid>/workers`
 * with the UMS worker as target.
 *
 * @param[in,out] dirs		pointer to the UMS scheduler procfs directories
 * @param[in,out] worker_dirs	pointer to the UMS worker procfs directories
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 */
int ums_scheduler_proc_register_worker(struct ums_scheduler_proc_dirs *dirs,
				struct ums_worker_proc_dirs *worker_dirs);

/**
 * @brief Unregister the UMS worker to the UMS scheduler procfs
 *
 * Remove the symlink inside `/proc/ums/<pid>/schedulers/<sched-pid>/workers`
 * with the UMS worker as target.
 *
 * @param[in,out] dirs		pointer to the UMS scheduler procfs directories
 * @param[in,out] worker_dirs	pointer to the UMS worker procfs directories
 */
void ums_scheduler_proc_unregister_worker(struct ums_scheduler_proc_dirs *dirs,
				struct ums_worker_proc_dirs *worker_dirs);

#endif /* PROC_SCHEDULER_H */
