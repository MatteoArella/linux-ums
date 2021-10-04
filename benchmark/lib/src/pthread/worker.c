// SPDX-License-Identifier: AGPL-3.0-only

#include "worker.h"

#include <stdlib.h>
#include <pthread.h>

struct pthread_worker {
	pthread_t thread;
	void *(*worker_routine)(task_t *task);
	task_t *task;
};

static void *worker_routine(void *args)
{
	struct pthread_worker *worker = args;

	return worker->worker_routine(worker->task);
}

pthread_worker_t *pthread_worker_create(pthread_worker_props_t *props)
{
	struct pthread_worker *worker;
	int retval;

	worker = malloc(sizeof(*worker));
	if (!worker)
		return NULL;

	worker->worker_routine = props->worker_routine;
	worker->task = props->task;

	retval = pthread_create(&worker->thread, NULL, worker_routine, worker);
	if (retval)
		goto out;

	return worker;

out:
	free(worker);
	return NULL;
}

int pthread_worker_destroy(pthread_worker_t *worker)
{
	int retval;

	retval = pthread_join(worker->thread, NULL);

	free(worker);

	return retval;
}
