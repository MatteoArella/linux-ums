/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "ums.h"

#include <linux/list.h>
#include <linux/sched.h>

struct ums_context {
	struct task_struct *task;
	struct list_head list;
};

int ums_context_create(struct ums_context *context);

int ums_context_init(struct ums_context *context);

int ums_context_destroy(struct ums_context *context);

#endif /* CONTEXT_H */
