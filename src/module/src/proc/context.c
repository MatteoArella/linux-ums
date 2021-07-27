// SPDX-License-Identifier: AGPL-3.0-only

#include "context.h"

static const char * const context_state_array[] = {
	"R (running)",			/* 0 */
	"I (idle)",			/* 1 */
};

const char *get_context_state(struct ums_context *context)
{
	unsigned int state = READ_ONCE(context->state);

	/* read barrier */
	smp_rmb();
	return context_state_array[fls(state)];
}
