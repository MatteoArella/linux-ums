// SPDX-License-Identifier: AGPL-3.0-only

#include "complist.h"
#include "worker.h"

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static inline struct ums_complist *alloc_ums_complist(void)
{
	return kzalloc(sizeof(struct ums_complist), GFP_KERNEL);
}

static inline void free_ums_complist(struct ums_complist *complist)
{
	kfree(complist);
}

static inline int ums_complist_init(struct ums_data *data,
				    struct ums_complist *complist)
{
	int retval;

	retval = IDR_L_ALLOC(&data->comp_lists, complist, GFP_KERNEL);
	if (unlikely(!retval))
		return retval;

	complist->id = retval;
	spin_lock_init(&complist->lock);
	INIT_LIST_HEAD(&complist->head);
	init_waitqueue_head(&complist->wait_q);

	return 0;
}

static inline void ums_complist_deinit(struct ums_data *data,
				    struct ums_complist *complist)
{
	IDR_L_REMOVE(&data->comp_lists, complist->id);
}

int create_ums_completion_list(struct ums_data *data,
			       ums_comp_list_id_t __user *id)
{
	struct ums_complist *complist;
	int retval;

	complist = alloc_ums_complist();
	if (unlikely(!complist)) {
		retval = -ENOMEM;
		goto alloc_complist;
	}

	retval = ums_complist_init(data, complist);
	if (unlikely(retval))
		goto complist_init;

	if (copy_to_user(id, &complist->id, sizeof(ums_comp_list_id_t))) {
		retval = -EACCES;
		goto copy_user;
	}

	return 0;

copy_user:
	ums_complist_deinit(data, complist);
complist_init:
	free_ums_complist(complist);
	complist = NULL;
alloc_complist:
	return retval;
}


int ums_complist_destroy(struct ums_complist *complist)
{
	wake_up_all(&complist->wait_q);
	free_ums_complist(complist);
	return 0;
}

void ums_completion_list_add(struct ums_complist *complist,
			     struct ums_context *context)
{
	spin_lock(&complist->lock);
	list_add_tail(&context->list, &complist->head);
	spin_unlock(&complist->lock);

	// notify new context
	wake_up_all(&complist->wait_q);
}

static inline struct ums_context *
do_dequeue_ums_complist_context(struct ums_complist *complist)
{
	struct ums_context *context;

	spin_lock(&complist->lock);

	while (list_empty(&complist->head)) {
		spin_unlock(&complist->lock);

		if (wait_event_interruptible(complist->wait_q,
					    !list_empty(&complist->head)))
			return ERR_PTR(-ERESTARTSYS);

		spin_lock(&complist->lock);
	}

	context = list_first_entry(&complist->head, struct ums_context, list);
	list_del(&context->list);

	// replace new head
	list_replace_init(&complist->head, &context->list);

	spin_unlock(&complist->lock);

	return context;
}

int ums_complist_dqcontext(struct ums_data *data,
			   struct dequeue_ums_complist_args __user *args)
{
	ums_comp_list_id_t complist_id;
	int retval;
	struct ums_complist *complist;
	struct ums_context *context;
	struct ums_worker *worker;

	retval = copy_from_user(&complist_id,
				&args->ums_complist,
				sizeof(ums_comp_list_id_t));
	if (unlikely(retval))
		return -EACCES;

	// find complist
	complist = IDR_L_FIND(&data->comp_lists, complist_id);
	if (unlikely(!complist))
		return -EINVAL;

	context = do_dequeue_ums_complist_context(complist);
	if (IS_ERR(context))
		return PTR_ERR(context);

	worker = container_of(context, struct ums_worker, context);

	if (copy_to_user(&args->ums_context,
			 &worker->id,
			 sizeof(ums_worker_id_t)))
		retval = -EACCES;

	return retval;
}

int ums_complist_next_context(struct ums_data *data,
			      struct ums_next_context_list_args __user *args)
{
	ums_worker_id_t worker_id;
	int retval;
	struct ums_context *context;
	struct ums_worker *worker;
	struct list_head *context_list;

	retval = copy_from_user(&worker_id,
				&args->ums_context,
				sizeof(ums_worker_id_t));
	if (unlikely(retval))
		return -EACCES;

	// find worker
	worker = IDR_L_FIND(&data->workers, worker_id);
	if (unlikely(!worker))
		return -EINVAL;

	context = &worker->context;
	context_list = &context->list;

	/*
	 * only one scheduler is owning the context list so we can access
	 * it with no synchronization
	 */
	if (list_empty(context_list)) {
		worker_id = -1;
		retval = copy_to_user(&args->ums_next_context,
				&worker_id,
				sizeof(ums_worker_id_t));
		if (unlikely(retval))
			return -EACCES;
		return 0;
	}

	context = list_first_entry(context_list, struct ums_context, list);
	worker = container_of(context, struct ums_worker, context);

	list_del(&context->list);

	// replace new head
	list_replace_init(context_list, &context->list);

	retval = copy_to_user(&args->ums_next_context,
				&worker->id,
				sizeof(ums_worker_id_t));
	if (unlikely(retval))
		return -EACCES;

	return 0;
}

int ums_complist_delete(struct ums_data *data, ums_comp_list_id_t complist_id)
{
	struct ums_complist *complist;

	// find complist
	complist = IDR_L_FIND(&data->comp_lists, complist_id);
	if (unlikely(!complist))
		return -EINVAL;

	return ums_complist_destroy(complist);
}
