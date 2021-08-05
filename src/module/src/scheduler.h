/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "uapi/ums/ums_ioctl.h"

#include "ums.h"
#include "context.h"
#include "complist.h"
#include "proc/scheduler.h"

#include <linux/list.h>

/**
 * Add a UMS scheduling event to the head of the event queue
 */
#define EVENT_ADD_HEAD		1U

/**
 * Add a UMS scheduling event to the tail of the event queue
 */
#define EVENT_ADD_TAIL		2U

/**
 * @brief UMS scheduler event node
 */
struct ums_event_node {
	struct ums_sched_event event;
	struct list_head list;
};

/**
 * UMS scheduler struct
 */
struct ums_scheduler {
	/**
	 * scheduler context
	 */
	struct ums_context context;

	/**
	 * scheduler completion list
	 */
	struct ums_complist *complist;

	/**
	 * scheduler event queue list
	 */
	struct list_head event_q;

	/**
	 * scheduler spinlock
	 */
	spinlock_t lock;

	/**
	 * scheduler wait queue
	 */
	wait_queue_head_t sched_wait_q;

	/**
	 * scheduler procfs dirs
	 */
	struct ums_scheduler_proc_dirs dirs;
};

/**
 * @brief Create a slab cache for allocating UMS scheduling events
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 */
int ums_scheduling_cache_create(void);

/**
 * @brief Destroy the slab cache for UMS scheduling events
 */
void ums_scheduling_cache_destroy(void);

/**
 * @brief Enter UMS scheduling mode
 *
 * Creates a UMS scheduler and send a #SCHEDULER_STARTUP event to it.
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
int enter_ums_scheduler_mode(struct ums_data *data,
			     struct enter_ums_mode_args *args);

/**
 * @brief Allocate an UMS scheduling event from the dedicated slab cache
 *
 * @return			the UMS scheduling event
 */
struct ums_event_node *alloc_ums_event(void);

/**
 * @brief Deallocate the UMS scheduling event
 */
void free_ums_event(struct ums_event_node *event);

/**
 * @brief Enqueue the UMS scheduling event to the UMS scheduler queue
 *
 * **Context**: Process context. Takes and releases @p scheduler->lock
 *
 * @param[in] scheduler		pointer to the UMS scheduler
 * @param[in] event		pointer to the UMS scheduling event
 * @param[in] flags		flags specifying how to add the event (can be
 *				one from #EVENT_ADD_HEAD or #EVENT_ADD_TAIL)
 */
void enqueue_ums_sched_event(struct ums_scheduler *scheduler,
			    struct ums_event_node *event,
			    unsigned int flags);

/**
 * @brief Dequeue a UMS scheduling event from the UMS scheduler queue
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *		Takes and releases @p scheduler->lock
 *
 * @param[in] data		pointer to the UMS data
 * @param[in] args		userspace pointer to the scheduling event
 *
 * @return
 * * 0				- OK
 * * -ESRCH			- Bad UMS calling thread
 * * -EACCES			- Bad userspace pointer
 * * -EINTR			- Interrupted call
 */
int ums_sched_dqevent(struct ums_data *data,
		      struct ums_sched_event __user *args);

/**
 * @brief Destroy the UMS scheduler and every UMS scheduling event associated
 *	  with it.
 *
 * **Context**: Process context. Takes and releases @p scheduler->lock
 *
 * @param[in] sched		pointer to the UMS scheduler
 */
void ums_scheduler_destroy(struct ums_scheduler *sched);

/**
 * @brief Execute a UMS worker context.
 *
 * Suspend the UMS scheduler switching its context with the UMS worker's one.
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *
 * @param[in] data		pointer to the UMS data
 * @param[in] worker_pid	pid of the UMS worker
 *
 * @return
 * * 0				- OK
 * * -ESRCH			- Bad worker pid or bad UMS calling thread
 */
int exec_ums_context(struct ums_data *data, pid_t worker_pid);

#endif /* SCHEDULER_H */
