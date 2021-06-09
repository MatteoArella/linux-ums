#ifndef CONTEXT_H
#define CONTEXT_H

#include <linux/list.h>
#include <linux/sched.h>

struct ums_context {
	u32 id;
	struct task_struct *task;
	struct list_head list;
};

int ums_context_create(struct ums_context *);

int ums_context_init(struct ums_context *);

int ums_context_destroy(struct ums_context *);

#endif /* CONTEXT_H */
