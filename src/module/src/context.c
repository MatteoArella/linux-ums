// SPDX-License-Identifier: AGPL-3.0-only

#include "context.h"

#include <linux/version.h>

#if KERNEL_VERSION(4, 11, 0) <= LINUX_VERSION_CODE
#include <linux/sched/task.h>
#endif

void ums_context_init(struct ums_context *context)
{
	get_task_struct(current);
	context->task = current;
	context->pid = current_context_pid();
	set_context_state(context, CONTEXT_RUNNING);
	atomic_set(&context->switches, 0);
}

void ums_context_deinit(struct ums_context *context)
{
	put_task_struct(context->task);
}

void prepare_switch_context(struct ums_context *from, struct ums_context *to)
{
	rcu_assign_pointer(to->parent, from);
	pr_debug("switching from %d to %d\n", from->pid, to->pid);

	atomic_inc(&from->switches);
	prepare_suspend_context(from);
	wake_up_context(to);
}
