// SPDX-License-Identifier: AGPL-3.0-only

#include "context.h"

void ums_context_init(struct ums_context *context)
{
	context->task = current;
	context->pid = current_context_pid();
}

void prepare_switch_context(struct ums_context *from, struct ums_context *to)
{
	rcu_assign_pointer(to->parent, from);
	pr_debug("switching from %d to %d\n", from->pid, to->pid);

	prepare_suspend_context(from);
	wake_up_context(to);
}
