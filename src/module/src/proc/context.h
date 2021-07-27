/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_CONTEXT_H
#define PROC_CONTEXT_H

#include "../context.h"
#include <linux/string.h>

const char *get_context_state(struct ums_context *context);

static inline int context_snprintf(char *buf, size_t size,
				   struct ums_context *context)
{
	return snprintf(buf, size,
			"Pid:\t%d\nState:\t%s\nSwitches:\t%d\n",
			context->pid,
			get_context_state(context),
			atomic_read(&context->switches));
}

#endif /* PROC_CONTEXT_H */
