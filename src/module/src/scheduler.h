/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "uapi/ums_ioctl.h"

#include "ums.h"
#include "context.h"
#include "complist.h"

#include <linux/list.h>

struct ums_event_node {
	struct ums_sched_event event;
	struct list_head list;
};

struct ums_scheduler {
	struct ums_context context;
	struct ums_complist *complist;
	struct list_head event_q;
	spinlock_t lock;
	wait_queue_head_t sched_wait_q;
};

int ums_scheduling_cache_create(void);
void ums_scheduling_cache_destroy(void);

int enter_ums_scheduler_mode(struct ums_data *data,
			     struct enter_ums_mode_args *args);

struct ums_event_node *alloc_ums_event(void);
void free_ums_event(struct ums_event_node *event);

void enqueue_ums_sched_event(struct ums_scheduler *scheduler,
			    struct ums_event_node *event);

void ums_scheduler_destroy(struct ums_scheduler *sched);

int exec_ums_context(struct ums_data *data, pid_t worker_pid);

#endif /* SCHEDULER_H */
