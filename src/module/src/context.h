/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "ums.h"

#include <linux/list.h>
#include <linux/rhashtable.h>
#include <linux/sched.h>

/**
 * UMS context running state
 */
#define CONTEXT_RUNNING		0

/**
 * UMS context idle state
 */
#define CONTEXT_IDLE		1

#ifndef set_task_state
#define set_task_state(tsk, state_value)			\
	/* write task state and perform memory barrier */	\
	smp_store_mb((tsk)->state, (state_value))
#endif

/**
 * @brief Set context state
 *
 * **Context**: Any context.
 *
 * @param[in] ctx		pointer to a UMS context
 * @param[in] state_value	new state value
 */
#define __set_context_state(ctx, state_value)			\
	((ctx)->state = (state_value))

/**
 * @brief Set context state and perform a write memory barrier
 *
 * **Context**: Any context.
 *
 * @param[in] ctx		pointer to a UMS context
 * @param[in] state_value	new state value
 */
#define set_context_state(ctx, state_value)			\
	/* write context state and perform memory barrier */	\
	smp_store_mb((ctx)->state, (state_value))

/**
 * UMS context structure
 */
struct ums_context {
	/**
	 * context state
	 */
	unsigned int state;

	/**
	 * <code>task_struct</code> associated with the UMS context
	 */
	struct task_struct *task;

	/**
	 * context key id
	 */
	pid_t pid;

	/**
	 * parent context
	 */
	struct ums_context __rcu *parent;

	/**
	 * context hashtable node
	 */
	struct rhash_head node;

	/**
	 * context switches counter
	 */
	atomic_t switches;

	/**
	 * head to context list forming a completion list
	 */
	struct list_head list;

	/**
	 * rcu head
	 */
	struct rcu_head rcu_head;

	/**
	 * pointer to UMS device private data where this context belongs to
	 */
	struct ums_data *data;
};

const static struct rhashtable_params ums_context_params = {
	.key_len     = sizeof(pid_t),
	.key_offset  = offsetof(struct ums_context, pid),
	.head_offset = offsetof(struct ums_context, node),
};

/**
 * @brief Initialize a UMS context
 *
 * Set the context task as `current` and increment its reference counter.
 *
 * The initialized context state is setted to #CONTEXT_RUNNING.
 *
 * **Context**: Any context.
 *
 * @param[in] context		pointer to a UMS context
 */
void ums_context_init(struct ums_context *context);

/**
 * @brief Deinitialize a UMS context
 *
 * Decrement context task reference counter.
 *
 * **Context**: Any context.
 *
 * @param[in] context		pointer to a UMS context
 */
void ums_context_deinit(struct ums_context *context);

/**
 * @brief Retrieve current UMS context pid
 *
 * **Context**: Any context.
 *
 * @return The virtual pid of the current UMS context.
 */
static inline pid_t current_context_pid(void)
{
	return task_pid_vnr(current);
}

/**
 * @brief Initialize suspending UMS context
 *
 * Change context state to #CONTEXT_IDLE
 *
 * **Context**: Any context.
 *
 * @param[in] context		pointer to a UMS context
 */
static inline void prepare_suspend_context(struct ums_context *context)
{
	__set_context_state(context, CONTEXT_IDLE);
	set_task_state(context->task, TASK_INTERRUPTIBLE);
}

/**
 * @brief Wake up an idle UMS context
 *
 * Change context state to #CONTEXT_RUNNING and wake up the context task.
 *
 * **Context**: Any context.
 *
 * @param[in] context		pointer to a UMS context
 */
static inline void wake_up_context(struct ums_context *context)
{
	set_context_state(context, CONTEXT_RUNNING);
	wake_up_process(context->task);
}

/**
 * @brief Initialize a UMS context switch
 *
 * Assign the @p from context to the @p to->parent;
 * suspend the @p from context and wake up the @p to context.
 *
 * **Context**: Any context.
 *
 * @param[in] from		pointer to the UMS context from which start
 *				the switch
 * @param[in] to		pointer to the UMS context to which end the
 *				switch
 */
void prepare_switch_context(struct ums_context *from, struct ums_context *to);

#endif /* CONTEXT_H */
