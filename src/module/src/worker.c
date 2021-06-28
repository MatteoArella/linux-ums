// SPDX-License-Identifier: AGPL-3.0-only

#include "worker.h"

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

	retval = IDR_L_ALLOC(&data->workers, worker, GFP_KERNEL);
	if (!retval)
		goto id_alloc;

	worker->id = retval;

	retval = ums_context_init(&worker->context);
	if (unlikely(retval))
		goto context_init;

	return 0;

context_init:
	IDR_L_REMOVE(&data->workers, worker->id);
id_alloc:
	return retval;
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
	worker = NULL;
alloc_worker:
	return ERR_PTR(retval);
}

int ums_worker_destroy(struct ums_worker *worker)
{
	int retval;

	retval = ums_context_destroy(&worker->context);
	free_ums_worker(worker);

	return retval;
}

static inline void suspend_worker(struct ums_worker *worker)
{
	set_current_state(TASK_INTERRUPTIBLE);
	schedule();
}


int enter_ums_worker_mode(struct ums_data *data,
			  struct enter_ums_mode_args *args)
{
	struct ums_worker *worker;
	int retval;

	worker = ums_worker_create(data);
	if (IS_ERR(worker)) {
		retval = PTR_ERR(worker);
		goto worker_create;
	}

	// TODO: get completion list and add worker context to it

	args->ums_worker = worker->id;

	suspend_worker(worker);

	return 0;

worker_create:
	return retval;
}
