// SPDX-License-Identifier: AGPL-3.0-only

#include "scheduler.h"

#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

struct kmem_cache *ums_event_cache;

int ums_scheduling_cache_create(void)
{
	ums_event_cache = KMEM_CACHE(ums_event, SLAB_HWCACHE_ALIGN);
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

inline struct ums_event *alloc_ums_event(void)
{
	return kmem_cache_zalloc(ums_event_cache, GFP_KERNEL);
}

static inline void free_ums_event(struct ums_event **event)
{
	kmem_cache_free(ums_event_cache, *event);
	*event = NULL;
}

static inline int ums_scheduler_init(struct ums_data *data,
				     struct ums_scheduler *sched)
{
	int retval;

	retval = IDR_L_ALLOC(&data->schedulers, sched, GFP_KERNEL);
	if (!retval)
		goto id_alloc;

	sched->id = retval;

	retval = ums_context_init(&sched->context);
	if (unlikely(retval))
		goto context_init;

	spin_lock_init(&sched->lock);

	INIT_LIST_HEAD(&sched->event_q);

	init_waitqueue_head(&sched->sched_wait_q);

	return 0;

context_init:
	IDR_L_REMOVE(&data->schedulers, sched->id);
id_alloc:
	return retval;
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
	scheduler = NULL;
alloc_scheduler:
	return ERR_PTR(retval);
}

int ums_scheduler_destroy(struct ums_scheduler *sched)
{
	struct list_head *curr;
	struct ums_event *event;
	int retval;

	spin_lock(&sched->lock);

	list_for_each(curr, &sched->event_q) {
		event = list_entry(curr, struct ums_event, list);
		list_del(&event->list);
		free_ums_event(&event);
	}

	retval = ums_context_destroy(&sched->context);

	spin_unlock(&sched->lock);

	free_ums_scheduler(sched);
	return 0;
}

int enter_ums_scheduler_mode(struct ums_data *data,
			     struct enter_ums_mode_args *args)
{
	struct ums_scheduler *scheduler;
	struct ums_event *startup_event;
	int retval;

	scheduler = ums_scheduler_create(data);
	if (IS_ERR(scheduler)) {
		retval = PTR_ERR(scheduler);
		goto sched_create;
	}

	// TODO: get completion list and assign it to scheduler

	args->ums_scheduler = scheduler->id;

	startup_event = alloc_ums_event();
	if (unlikely(!startup_event)) {
		retval = -ENOMEM;
		goto startup_event_alloc;
	}

	startup_event->type = SCHEDULER_STARTUP;

	retval = enqueue_ums_sched_event(scheduler, startup_event);
	if (retval)
		goto enqueue_startup_sched_event;

	return 0;

enqueue_startup_sched_event:
	free_ums_event(&startup_event);
startup_event_alloc:
	ums_scheduler_destroy(scheduler);
	scheduler = NULL;
sched_create:
	return retval;
}

int enqueue_ums_sched_event(struct ums_scheduler *scheduler,
			    struct ums_event *event)
{
	spin_lock(&scheduler->lock);
	list_add_tail(&event->list, &scheduler->event_q);
	spin_unlock(&scheduler->lock);

	// notify new event
	wake_up_all(&scheduler->sched_wait_q);

	return 0;
}

static inline struct ums_event *
do_dequeue_ums_sched_event(struct ums_scheduler *scheduler)
{
	struct ums_event *event;

	spin_lock(&scheduler->lock);

	while (list_empty(&scheduler->event_q)) {
		spin_unlock(&scheduler->lock);

		if (wait_event_interruptible(scheduler->sched_wait_q,
					     !list_empty(&scheduler->event_q)))
			return ERR_PTR(-ERESTARTSYS);

		spin_lock(&scheduler->lock);
	}

	// consume the event
	event = list_first_entry(&scheduler->event_q, struct ums_event, list);
	list_del(scheduler->event_q.next);

	spin_unlock(&scheduler->lock);

	return event;
}

int ums_sched_dqevent(struct ums_data *data,
		      struct ums_sched_dqevent_args __user *args)
{
	ums_sched_id_t sched_id;
	int retval;
	struct ums_scheduler *scheduler;
	struct ums_event *e;
	struct ums_sched_event kevent;

	retval = copy_from_user(&sched_id,
				&args->ums_scheduler,
				sizeof(ums_sched_id_t));
	if (unlikely(retval))
		return -EACCES;

	// find scheduler
	scheduler = IDR_L_FIND(&data->schedulers, sched_id);

	e = do_dequeue_ums_sched_event(scheduler);
	if (IS_ERR(e))
		return PTR_ERR(e);

	kevent.type = e->type;

	if (copy_to_user(&args->event,
			 &kevent,
			 sizeof(struct ums_sched_event)))
		retval = -EACCES; // TODO: fix event lost

	free_ums_event(&e);

	return retval;
}
