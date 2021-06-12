/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_COMPLIST_H
#define UMS_COMPLIST_H

#include "ums.h"
#include "context.h"

#include <linux/list.h>

struct ums_complist {
	ums_comp_list_id_t id;
	/* list_head to first ums_context */
	struct list_head head;
	spinlock_t lock;
	wait_queue_head_t wait_q;
};

int create_ums_completion_list(struct ums_data *data,
			       ums_comp_list_id_t __user *id);

void ums_completion_list_add(struct ums_complist *complist,
			     struct ums_context *context);

int ums_complist_dqcontext(struct ums_data *data,
			   struct dequeue_ums_complist_args __user *args);

int ums_complist_next_context(struct ums_data *data,
			      struct ums_next_context_list_args __user *args);

int ums_complist_delete(struct ums_data *data, ums_comp_list_id_t complist_id);

int ums_complist_destroy(struct ums_complist *complist);

#endif /* UMS_COMPLIST_H */
