/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "ums.h"

#include <linux/list.h>
#include <linux/rhashtable.h>
#include <linux/sched.h>

#define CONTEXT_RUNNING		0
#define CONTEXT_IDLE		1

#ifndef set_task_state
#define set_task_state(tsk, state_value)			\
	/* write task state and perform memory barrier */	\
	smp_store_mb((tsk)->state, (state_value))
#endif

#define __set_context_state(ctx, state_value)			\
	((ctx)->state = (state_value))

#define set_context_state(ctx, state_value)			\
	/* write context state and perform memory barrier */	\
	smp_store_mb((ctx)->state, (state_value))

struct ums_context {
	unsigned int state;
	struct task_struct *task;
	pid_t pid; /* rhashtable key */
	struct ums_context __rcu *parent;
	struct rhash_head node;
	atomic_t switches;
	struct list_head list;
	struct rcu_head rcu_head;
	struct ums_data *data;
};

const static struct rhashtable_params ums_context_params = {
	.key_len     = sizeof(pid_t),
	.key_offset  = offsetof(struct ums_context, pid),
	.head_offset = offsetof(struct ums_context, node),
};

void ums_context_init(struct ums_context *context);
void ums_context_deinit(struct ums_context *context);

static inline pid_t current_context_pid(void)
{
	return task_pid_vnr(current);
}

static inline void prepare_suspend_context(struct ums_context *context)
{
	__set_context_state(context, CONTEXT_IDLE);
	set_task_state(context->task, TASK_INTERRUPTIBLE);
}

static inline void wake_up_context(struct ums_context *context)
{
	set_context_state(context, CONTEXT_RUNNING);
	wake_up_process(context->task);
}

void prepare_switch_context(struct ums_context *from, struct ums_context *to);

#endif /* CONTEXT_H */
