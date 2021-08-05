// SPDX-License-Identifier: AGPL-3.0-only

#include "worker.h"
#include "scheduler.h"

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/sched.h>

static inline struct ums_worker *alloc_ums_worker(void)
{
	return kmalloc(sizeof(struct ums_worker), GFP_KERNEL);
}

static inline void free_ums_worker(struct ums_worker *worker)
{
	kfree(worker);
}

static inline int ums_worker_init(struct ums_data *data,
				  struct ums_worker *worker)
{
	int retval;

	ums_context_init(&worker->context);
	worker->context.data = data;

	retval = ums_worker_proc_register(&data->dirs, worker);
	if (unlikely(retval))
		return retval;

	return rhashtable_insert_fast(&data->workers,
				      &worker->context.node,
				      ums_context_params);
}

static inline struct ums_worker *ums_worker_create(struct ums_data *data)
{
	struct ums_worker *worker;
	long retval;

	worker = alloc_ums_worker();
	if (unlikely(!worker)) {
		retval = -ENOMEM;
		goto alloc_worker;
	}

	retval = ums_worker_init(data, worker);
	if (unlikely(retval))
		goto worker_init;

	return worker;

worker_init:
	free_ums_worker(worker);
alloc_worker:
	return ERR_PTR(retval);
}

void ums_worker_call_rcu(struct rcu_head *head)
{
	struct ums_context *context;
	struct ums_worker *worker;

	context = container_of(head, struct ums_context, rcu_head);
	worker = container_of(context, struct ums_worker, context);

	ums_context_deinit(context);

	free_ums_worker(worker);
}

void ums_worker_destroy(struct ums_worker *worker)
{
	put_ums_complist(worker->complist);

	ums_worker_proc_unregister(worker);

	rhashtable_remove_fast(&worker->context.data->workers,
			       &worker->context.node,
			       ums_context_params);
	call_rcu(&worker->context.rcu_head, ums_worker_call_rcu);
}

int enter_ums_worker_mode(struct ums_data *data,
			  struct enter_ums_mode_args *args)
{
	struct ums_worker *worker;
	struct ums_complist *complist;
	int retval;

	worker = ums_worker_create(data);
	if (IS_ERR(worker)) {
		retval = PTR_ERR(worker);
		goto worker_create;
	}

	rcu_read_lock();
	complist = IDR_L_FIND(&data->comp_lists, args->ums_complist);
	if (unlikely(!complist)) {
		retval = -EINVAL;
		goto complist_find;
	}

	get_ums_complist(complist);
	rcu_read_unlock();

	worker->complist = complist;

	prepare_suspend_context(&worker->context);
	ums_completion_list_add(complist, &worker->context, COMPLIST_ADD_TAIL);
	schedule();

	return 0;

complist_find:
	rcu_read_unlock();
	ums_worker_destroy(worker);
worker_create:
	return retval;
}

int ums_worker_yield(struct ums_data *data, void __user *args)
{
	pid_t context_pid;
	struct ums_context *sched_context;
	struct ums_context *worker_context;
	struct ums_worker *worker;
	struct ums_scheduler *scheduler;
	struct ums_event_node *kevent;

	// find worker
	context_pid = current_context_pid();

	rcu_read_lock();
	worker_context = rhashtable_lookup_fast(&data->workers,
						&context_pid,
						ums_context_params);
	if (unlikely(!worker_context)) {
		rcu_read_unlock();
		return -ESRCH;
	}

	kevent = alloc_ums_event();
	if (unlikely(!kevent)) {
		rcu_read_unlock();
		return -ENOMEM;
	}

	worker = container_of(worker_context, struct ums_worker, context);

	kevent->event.type = THREAD_YIELD;
	kevent->event.yield_params.context = context_pid;
	kevent->event.yield_params.scheduler_params = args;

	sched_context = rcu_dereference(worker_context->parent);

	scheduler = container_of(sched_context, struct ums_scheduler, context);
	enqueue_ums_sched_event(scheduler, kevent, EVENT_ADD_TAIL);

	prepare_switch_context(worker_context, sched_context);

	ums_completion_list_add(worker->complist,
				worker_context,
				COMPLIST_ADD_TAIL);

	rcu_read_unlock();

	schedule();
	return 0;
}

int ums_worker_end(struct ums_data *data)
{
	pid_t context_pid;
	struct ums_context *sched_context;
	struct ums_context *worker_context;
	struct ums_scheduler *scheduler;
	struct ums_worker *worker;
	struct ums_event_node *kevent;
	int retval;

	// find worker context
	context_pid = current_context_pid();

	rcu_read_lock();
	worker_context = rhashtable_lookup_fast(&data->workers,
						&context_pid,
						ums_context_params);
	if (unlikely(!worker_context)) {
		rcu_read_unlock();
		return -ESRCH;
	}

	kevent = alloc_ums_event();
	if (unlikely(!kevent)) {
		rcu_read_unlock();
		return -ENOMEM;
	}

	sched_context = rcu_dereference(worker_context->parent);
	if (unlikely(!sched_context)) {
		retval = -ESRCH;
		goto out;
	}

	scheduler = container_of(sched_context, struct ums_scheduler, context);
	worker = container_of(worker_context, struct ums_worker, context);

	kevent->event.type = THREAD_TERMINATED;
	kevent->event.end_params.context = context_pid;

	enqueue_ums_sched_event(scheduler, kevent, EVENT_ADD_TAIL);
	wake_up_context(sched_context);

	ums_worker_destroy(worker);
	rcu_read_unlock();

	return 0;
out:
	rcu_read_unlock();
	free_ums_event(kevent);
	return retval;
}
