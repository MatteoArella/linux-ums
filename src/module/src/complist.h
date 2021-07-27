/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_COMPLIST_H
#define UMS_COMPLIST_H

#include "ums.h"
#include "context.h"

#include <linux/list.h>
#include <linux/kref.h>

#define COMPLIST_ADD_HEAD		1U
#define COMPLIST_ADD_TAIL		2U

struct ums_complist {
	ums_comp_list_id_t id;
	/* list_head to first ums_context */
	struct list_head head;
	spinlock_t lock;
	wait_queue_head_t wait_q;
	struct kref refcount;
	struct rcu_head rhead;
	struct ums_data *data;
};

int create_ums_completion_list(struct ums_data *data,
			       ums_comp_list_id_t __user *id);

void ums_completion_list_add(struct ums_complist *complist,
			     struct ums_context *context, unsigned int flags);

int ums_complist_dqcontext(struct ums_data *data,
			   struct dequeue_ums_complist_args __user *args);

int ums_complist_next_context(struct ums_data *data,
			      struct ums_next_context_list_args __user *args);

static inline struct ums_complist *get_ums_complist(struct ums_complist *c)
{
	kref_get(&c->refcount);
	return c;
}

int put_ums_complist(struct ums_complist *complist);

int ums_complist_delete(struct ums_data *data, ums_comp_list_id_t complist_id);

#endif /* UMS_COMPLIST_H */
