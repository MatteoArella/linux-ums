/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef PROC_CONTEXT_H
#define PROC_CONTEXT_H

#include "../context.h"
#include <linux/string.h>

/**
 * @brief Get a string representation of the UMS context state
 *
 * @param[in] context		pointer to the UMS context
 *
 * @return			the context state representation
 */
const char *get_context_state(struct ums_context *context);

/**
 * @brief Get a string representation of the UMS context
 *
 * @param[in,out] buf		pointer to an allocated buffer
 * @param[in] size		maximum number of bytes to write
 * @param[in] context		pointer to the UMS context
 *
 * @return			the number of characters printed or truncated
 */
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
