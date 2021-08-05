/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_COMPLIST_H
#define UMS_COMPLIST_H

#include "ums.h"
#include "context.h"

#include <linux/list.h>
#include <linux/kref.h>

/**
 * Add a UMS context to the head of the UMS completion list
 */
#define COMPLIST_ADD_HEAD		1U

/**
 * Add a UMS context to the tail of the UMS completion list
 */
#define COMPLIST_ADD_TAIL		2U

/**
 * UMS completion list structure
 */
struct ums_complist {
	/**
	 * UMS completion list id
	 */
	ums_comp_list_id_t id;

	/**
	 * list head to first UMS context
	 */
	struct list_head head;

	spinlock_t lock;

	/**
	 * wait queue for dequeuing UMS contexts
	 */
	wait_queue_head_t wait_q;

	/**
	 * Reference counter
	 */
	struct kref refcount;

	/**
	 * rcu head
	 */
	struct rcu_head rhead;

	/**
	 * pointer to UMS device private data where this context belongs to
	 */
	struct ums_data *data;
};

/**
 * @brief Create a UMS completion list
 *
 * Allocates a @ref ums_complist and initialize all its data structures.
 *
 * The @ref ums_complist.refcount counter is incremented and its
 * @ref ums_complist.id is passed to the user.
 *
 * **Context**: Process context. May sleep.
 *
 * @param[in] data		pointer to the UMS data
 * @param[in] id		userspace pointer to the completion list id
 *
 * @return
 * * 0				- OK
 * * -ENOMEM			- No memory available
 * * -EACCESS			- Bad userspace pointer
 */
int create_ums_complist(struct ums_data *data,
			ums_comp_list_id_t __user *id);

/**
 * @brief Add a UMS context to a UMS completion list
 *
 * **Context**: Process context. Takes and releases @p complist->lock.
 *
 * @param[in] complist		pointer to the UMS completion list
 * @param[in] context		pointer to the UMS worker context
 * @param[in] flags		flags specifying how to add the context (can be
 *				one from #COMPLIST_ADD_HEAD or
 *				#COMPLIST_ADD_TAIL)
 */
void ums_completion_list_add(struct ums_complist *complist,
			     struct ums_context *context, unsigned int flags);

/**
 * @brief Retrieve a UMS context from a UMS completion list
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *		Takes and releases @p complist->lock.
 *
 * @param[in] data		pointer to the UMS private data
 * @param[in] args		Userspace pointer for args
 *
 * @return
 * * 0				- OK
 * * -EACCESS			- Bad userspace pointer
 * * -EINVAL			- Bad completion list id
 * * -ESRCH			- Bad UMS calling thread
 * * -EINTR			- Interrupted call
 */
int ums_complist_dqcontext(struct ums_data *data,
			   struct dequeue_ums_complist_args __user *args);

/**
 * @brief Retrieve the next UMS context from a UMS thread list
 *
 * **Context**: Process context. May sleep. Takes and releases the RCU lock.
 *
 * @param[in] data		pointer to the UMS private data
 * @param[in] args		Userspace pointer for args
 *
 * @return
 * * 0				- OK
 * * -EACCESS			- Bad userspace pointer
 * * -ESRCH			- Bad UMS context
 */
int ums_complist_next_context(struct ums_data *data,
			      struct ums_next_context_list_args __user *args);

/**
 * @brief Increment the UMS completion list reference counter
 *
 * **Context**: Any context.
 *
 * @param[in] c			pointer to the UMS completion list
 *
 * @return			the UMS completion list
 */
static inline struct ums_complist *get_ums_complist(struct ums_complist *c)
{
	kref_get(&c->refcount);
	return c;
}

/**
 * @brief Decrement the UMS completion list reference counter and destroy it if
 *	  the counter reaches zero.
 *
 * **Context**: Any context.
 *
 * @param[in] complist		pointer to the UMS completion list
 *
 * @return			1 if the completion list is destroyed,
 *				0 otherwise.
 */
int put_ums_complist(struct ums_complist *complist);

/**
 * @brief Delete the UMS completion list.
 *
 * Decrement the UMS completion list reference counter and destroy it if
 * the counter reaches zero.
 *
 * **Context**: Any context. Takes and releases the RCU lock.
 *
 * @param[in] data		pointer to the UMS private data
 * @param[in] complist_id	id of the UMS completion list
 *
 * @return
 * * 0				- OK
 * * -EINVAL			- Bad UMS completion list id
 */
int ums_complist_delete(struct ums_data *data, ums_comp_list_id_t complist_id);

#endif /* UMS_COMPLIST_H */
