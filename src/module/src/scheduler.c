// SPDX-License-Identifier: AGPL-3.0-only

#include "scheduler.h"
#include "worker.h"
#include "proc/scheduler.h"

#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

struct kmem_cache *ums_event_cache;

int ums_scheduling_cache_create(void)
{
	ums_event_cache = KMEM_CACHE(ums_event_node, SLAB_HWCACHE_ALIGN);
	return ums_event_cache != NULL ? 0 : -ENOMEM;
}

void ums_scheduling_cache_destroy(void)
{
	kmem_cache_destroy(ums_event_cache);
}

static inline struct ums_scheduler *alloc_ums_scheduler(void)
{
	return kmalloc(sizeof(struct ums_scheduler), GFP_KERNEL);
}

static inline void free_ums_scheduler(struct ums_scheduler *scheduler)
{
	kfree(scheduler);
}

struct ums_event_node *alloc_ums_event(void)
{
	return kmem_cache_zalloc(ums_event_cache, GFP_KERNEL);
}

void free_ums_event(struct ums_event_node *event)
{
	kmem_cache_free(ums_event_cache, event);
}

void ums_scheduler_release(struct ums_context *context);

static inline int ums_scheduler_init(struct ums_data *data,
				     struct ums_scheduler *sched)
{
	int retval;

	ums_context_init(&sched->context);
	sched->context.data = data;
	sched->context.release = ums_scheduler_release;

	spin_lock_init(&sched->lock);
	INIT_LIST_HEAD(&sched->event_q);
	init_waitqueue_head(&sched->sched_wait_q);

	retval = ums_scheduler_proc_register(&data->dirs, sched);
	if (retval)
		return retval;

	return rhashtable_insert_fast(&data->context_table,
				      &sched->context.node,
				      ums_context_params);

}

static inline struct ums_scheduler *ums_scheduler_create(struct ums_data *data)
{
	struct ums_scheduler *scheduler;
	long retval;

	scheduler = alloc_ums_scheduler();
	if (unlikely(!scheduler)) {
		retval = -ENOMEM;
		goto alloc_scheduler;
	}

	retval = ums_scheduler_init(data, scheduler);
	if (unlikely(retval))
		goto sched_init;

	return scheduler;

sched_init:
	free_ums_scheduler(scheduler);
alloc_scheduler:
	return ERR_PTR(retval);
}

void ums_scheduler_call_rcu(struct rcu_head *head)
{
	struct ums_context *context;
	struct ums_scheduler *scheduler;

	context = container_of(head, struct ums_context, rcu_head);
	scheduler = container_of(context, struct ums_scheduler, context);

	ums_context_deinit(context);

	free_ums_scheduler(scheduler);
}

void ums_scheduler_destroy(struct ums_scheduler *sched)
{
	struct ums_event_node *event, *tmp;

	spin_lock(&sched->lock);

	list_for_each_entry_safe(event, tmp, &sched->event_q, list) {
		list_del(&event->list);
		free_ums_event(event);
	}

	spin_unlock(&sched->lock);

	put_ums_complist(sched->complist);

	ums_scheduler_proc_unregister(sched);

	rhashtable_remove_fast(&sched->context.data->context_table,
			       &sched->context.node,
			       ums_context_params);
	call_rcu(&sched->context.rcu_head, ums_scheduler_call_rcu);
}

void ums_scheduler_release(struct ums_context *context)
{
	struct ums_scheduler *scheduler;

	scheduler = container_of(context, struct ums_scheduler, context);

	ums_scheduler_destroy(scheduler);
}

int enter_ums_scheduler_mode(struct ums_data *data,
			     struct enter_ums_mode_args *args)
{
	struct ums_scheduler *scheduler;
	struct ums_complist *complist;
	struct ums_event_node *startup_event;
	int retval;

	scheduler = ums_scheduler_create(data);
	if (IS_ERR(scheduler)) {
		retval = PTR_ERR(scheduler);
		goto sched_create;
	}

	rcu_read_lock();
	complist = IDR_L_FIND(&data->comp_lists, args->ums_complist);
	if (unlikely(!complist)) {
		retval = -EINVAL;
		goto complist_find;
	}

	get_ums_complist(complist);
	rcu_read_unlock();

	scheduler->complist = complist;

	startup_event = alloc_ums_event();
	if (unlikely(!startup_event)) {
		retval = -ENOMEM;
		goto complist_find;
	}

	startup_event->event.type = SCHEDULER_STARTUP;

	enqueue_ums_sched_event(scheduler, startup_event,
				EVENT_ADD_TAIL);

	return 0;

complist_find:
	rcu_read_unlock();
	ums_scheduler_destroy(scheduler);
sched_create:
	return retval;
}

void enqueue_ums_sched_event(struct ums_scheduler *scheduler,
			    struct ums_event_node *event,
			    unsigned int flags)
{
	spin_lock(&scheduler->lock);
	if (flags & EVENT_ADD_HEAD)
		list_add(&event->list, &scheduler->event_q);
	else if (flags & EVENT_ADD_TAIL)
		list_add_tail(&event->list, &scheduler->event_q);
	spin_unlock(&scheduler->lock);

	// notify new event
	wake_up_all(&scheduler->sched_wait_q);
}

static inline struct ums_event_node *
do_dequeue_ums_sched_event(struct ums_scheduler *scheduler)
{
	struct ums_event_node *event;

	spin_lock(&scheduler->lock);

	while (list_empty(&scheduler->event_q)) {
		spin_unlock(&scheduler->lock);

		if (wait_event_interruptible(scheduler->sched_wait_q,
					     !list_empty(&scheduler->event_q)))
			return ERR_PTR(-ERESTARTSYS);

		spin_lock(&scheduler->lock);
	}

	// consume the event
	event = list_first_entry(&scheduler->event_q,
				 struct ums_event_node,
				 list);
	list_del(&event->list);

	spin_unlock(&scheduler->lock);

	return event;
}

int ums_sched_dqevent(struct ums_data *data,
		      struct ums_sched_event __user *args)
{
	int retval;
	pid_t context_pid;
	struct ums_scheduler *scheduler;
	struct ums_context *sched_context;
	struct ums_event_node *e;

	// find scheduler
	context_pid = current_context_pid();

	rcu_read_lock();
	sched_context = rhashtable_lookup_fast(&data->context_table,
					       &context_pid,
					       ums_context_params);
	if (unlikely(!sched_context)) {
		retval = -ESRCH;
		goto out;
	}

	scheduler = container_of(sched_context, struct ums_scheduler, context);

	e = do_dequeue_ums_sched_event(scheduler);
	if (IS_ERR(e)) {
		retval = PTR_ERR(e);
		goto out;
	}

	retval = copy_to_user(args,
			      &e->event,
			      sizeof(struct ums_sched_event));
	if (unlikely(retval)) {
		retval = -EACCES;
		goto copy_user;
	}

	rcu_read_unlock();

	free_ums_event(e);

	return 0;
copy_user:
	enqueue_ums_sched_event(scheduler, e, EVENT_ADD_HEAD);
out:
	rcu_read_unlock();
	return retval;
}

int exec_ums_context(struct ums_data *data, pid_t worker_pid)
{
	pid_t context_pid;
	struct ums_context *sched_context;
	struct ums_context *worker_context;
	struct ums_scheduler *scheduler;
	struct ums_worker *worker;
	int retval;

	// find worker
	rcu_read_lock();
	worker_context = rhashtable_lookup_fast(&data->context_table,
						&worker_pid,
						ums_context_params);
	if (unlikely(!worker_context)) {
		retval = -ESRCH;
		goto out;
	}

	// find current scheduler
	context_pid = current_context_pid();

	sched_context = rhashtable_lookup_fast(&data->context_table,
					       &context_pid,
					       ums_context_params);
	if (unlikely(!sched_context)) {
		retval = -ESRCH;
		goto out;
	}

	scheduler = container_of(sched_context, struct ums_scheduler, context);
	worker = container_of(worker_context, struct ums_worker, context);

	ums_scheduler_proc_unregister_worker(&scheduler->dirs, &worker->dirs);

	prepare_switch_context(sched_context, worker_context);
	rcu_read_unlock();

	schedule();
	return 0;
out:
	rcu_read_unlock();
	return retval;
}
