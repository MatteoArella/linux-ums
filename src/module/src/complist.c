// SPDX-License-Identifier: AGPL-3.0-only

#include "complist.h"
#include "worker.h"
#include "scheduler.h"
#include "proc/scheduler.h"

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
	complist->data = data;
	spin_lock_init(&complist->lock);
	INIT_LIST_HEAD(&complist->head);
	init_waitqueue_head(&complist->wait_q);
	kref_init(&complist->refcount);

	return 0;
}

static inline void ums_complist_deinit(struct ums_data *data,
				    struct ums_complist *complist)
{
	IDR_L_REMOVE(&data->comp_lists, complist->id);
}

int create_ums_complist(struct ums_data *data,
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
alloc_complist:
	return retval;
}

static void ums_complist_call_rcu(struct rcu_head *rhead)
{
	free_ums_complist(container_of(rhead, struct ums_complist, rhead));
}

static int ums_complist_destroy(struct ums_complist *complist)
{
	wake_up_all(&complist->wait_q);
	ums_complist_deinit(complist->data, complist);
	call_rcu(&complist->rhead, ums_complist_call_rcu);
	return 0;
}

static void ums_complist_release(struct kref *kref)
{
	struct ums_complist *complist;

	complist = container_of(kref, struct ums_complist, refcount);
	ums_complist_destroy(complist);
}

int put_ums_complist(struct ums_complist *complist)
{
	return kref_put(&complist->refcount, ums_complist_release);
}

void ums_completion_list_add(struct ums_complist *complist,
			     struct ums_context *context, unsigned int flags)
{
	spin_lock(&complist->lock);
	if (flags & COMPLIST_ADD_HEAD)
		list_add(&context->list, &complist->head);
	else if (flags & COMPLIST_ADD_TAIL)
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

	pr_debug("dequeued context %d\n", context->pid);

	return context;
}

int ums_complist_dqcontext(struct ums_data *data,
			   struct dequeue_ums_complist_args __user *args)
{
	ums_comp_list_id_t complist_id;
	struct ums_complist *complist;
	struct ums_context *context, *curr;
	struct ums_context *sched_context;
	struct ums_scheduler *scheduler;
	struct ums_worker *worker;
	pid_t scheduler_pid;
	int retval;

	retval = copy_from_user(&complist_id,
				&args->ums_complist,
				sizeof(ums_comp_list_id_t));
	if (unlikely(retval))
		return -EACCES;

	// find complist
	rcu_read_lock();
	complist = IDR_L_FIND(&data->comp_lists, complist_id);
	if (unlikely(!complist)) {
		retval = -EINVAL;
		goto out;
	}

	scheduler_pid = current_context_pid();

	sched_context = rhashtable_lookup_fast(&data->schedulers,
					       &scheduler_pid,
					       ums_context_params);
	if (unlikely(!sched_context)) {
		retval = -ESRCH;
		goto out;
	}

	scheduler = container_of(sched_context, struct ums_scheduler, context);

	context = do_dequeue_ums_complist_context(complist);
	if (IS_ERR(context)) {
		retval = PTR_ERR(context);
		goto out;
	}

	// create one worker proc symlink for each context list entry
	worker = container_of(context, struct ums_worker, context);

	retval = ums_scheduler_proc_register_worker(&scheduler->dirs,
						    &worker->dirs);
	if (unlikely(retval))
		goto register_worker;

	list_for_each_entry(curr, &context->list, list) {
		worker = container_of(curr, struct ums_worker, context);
		ums_scheduler_proc_register_worker(&scheduler->dirs,
						   &worker->dirs);
	}

	if (copy_to_user(&args->ums_context,
			 &context->pid,
			 sizeof(pid_t))) {
		retval = -EACCES;
		goto register_worker;
	}

	rcu_read_unlock();
	return 0;

register_worker:
	ums_completion_list_add(complist, context, COMPLIST_ADD_HEAD);
out:
	rcu_read_unlock();
	return retval;
}

int ums_complist_next_context(struct ums_data *data,
			      struct ums_next_context_list_args __user *args)
{
	pid_t worker_pid;
	struct ums_context *context;
	struct list_head *context_list;
	int retval = 0;

	retval = copy_from_user(&worker_pid,
				&args->ums_context,
				sizeof(pid_t));
	if (unlikely(retval))
		return -EACCES;

	// find worker
	rcu_read_lock();
	context = rhashtable_lookup_fast(&data->workers,
					 &worker_pid,
					 ums_context_params);
	if (unlikely(!context)) {
		retval = -ESRCH;
		goto out;
	}

	context_list = &context->list;

	/*
	 * only one scheduler is owning the context list so we can access
	 * it with no synchronization
	 */
	if (list_empty(context_list)) {
		worker_pid = -1;
		retval = copy_to_user(&args->ums_next_context,
				&worker_pid,
				sizeof(pid_t));
		if (unlikely(retval))
			retval = -EACCES;
		goto out;
	}

	context = list_first_entry(context_list, struct ums_context, list);
	list_del(&context->list);

	// replace new head
	list_replace_init(context_list, &context->list);

	pr_debug("dequeued context %d\n", context->pid);

	retval = copy_to_user(&args->ums_next_context,
			      &context->pid,
			      sizeof(pid_t));
	if (unlikely(retval))
		retval = -EACCES;
out:
	rcu_read_unlock();
	return retval;
}

int ums_complist_delete(struct ums_data *data, ums_comp_list_id_t complist_id)
{
	struct ums_complist *complist;
	int retval = 0;

	// find complist
	rcu_read_lock();
	complist = IDR_L_FIND(&data->comp_lists, complist_id);
	if (unlikely(!complist)) {
		retval = -EINVAL;
		goto out;
	}

	put_ums_complist(complist);
out:
	rcu_read_unlock();
	return retval;
}
