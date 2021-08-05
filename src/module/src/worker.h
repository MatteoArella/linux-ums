/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef WORKER_H
#define WORKER_H

#include "uapi/ums/ums_ioctl.h"

#include "ums.h"
#include "context.h"
#include "complist.h"
#include "proc/worker.h"

/**
 * UMS worker struct
 */
struct ums_worker {
	/**
	 * worker context
	 */
	struct ums_context context;

	/**
	 * worker completion list
	 */
	struct ums_complist *complist;

	/**
	 * worker procfs dirs
	 */
	struct ums_worker_proc_dirs dirs;
};

/**
 * @brief Enter UMS worker mode
 *
 * Creates a UMS worker, add its UMS context to the UMS completion list and
 * suspend the UMS worker.
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *
 * @param[in] data		pointer to the UMS data
 * @param[in] args		userspace pointer for args
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 * * -EINVAL			- Invalid UMS completion list arg
 */
int enter_ums_worker_mode(struct ums_data *data,
			  struct enter_ums_mode_args *args);

/**
 * @brief Yield UMS worker
 *
 * Add the UMS worker context to the UMS completion list, enqueue a UMS
 * scheduling event of type #THREAD_YIELD to the parent UMS scheduler and
 * switch the UMS context with the parent one.
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *
 * @param[in] data		pointer to the UMS data
 * @param[in] args		userspace pointer for args
 *
 * @return
 * * 0				- OK
 * * -ESRCH			- Bad UMS calling thread
 * * -ENOMEM			- No memory available
 */
int ums_worker_yield(struct ums_data *data, void __user *args);

/**
 * @brief Terminate a UMS worker
 *
 * Enqueue a UMS scheduling event of type #THREAD_TERMINATED to the parent UMS
 * scheduler, destroy the UMS worker and wake up the parent UMS scheduler.
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *
 * @param[in] data		pointer to the UMS data
 *
 * @return
 * * 0				- OK
 * * -ESRCH			- Bad UMS calling thread
 * * -ENOMEM			- No memory available
 */
int ums_worker_end(struct ums_data *data);

/**
 * @brief Destroy the UMS worker.
 *
 * **Context**: Process context.
 *
 * @param[in] worker		pointer to the UMS worker
 */
void ums_worker_destroy(struct ums_worker *worker);

#endif /* WORKER_H */
