// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "worker.h"

#include <stdlib.h>
#include <pthread.h>

struct ums_worker {
	pthread_t thread;
	void *(*worker_routine)(task_t *task);
	task_t *task;
};

static void *worker_routine(void *args)
{
	struct ums_worker *worker = args;

	return worker->worker_routine(worker->task);
}

ums_worker_t *ums_worker_create(ums_worker_props_t *props)
{
	struct ums_worker *worker;
	int retval;

	worker = malloc(sizeof(*worker));
	if (!worker)
		return NULL;

	worker->worker_routine = props->worker_routine;
	worker->task = props->task;

	retval = ums_pthread_create(&worker->thread, props->attr,
				    worker_routine, worker);
	if (retval)
		goto out;

	return worker;

out:
	free(worker);
	return NULL;
}

int ums_worker_destroy(ums_worker_t *worker)
{
	int retval;

	retval = pthread_join(worker->thread, NULL);

	free(worker);

	return retval;
}
