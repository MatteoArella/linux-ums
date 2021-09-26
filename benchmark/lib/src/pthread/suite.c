// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "ums/benchmark/pthread_suite.h"
#include "../suite.h"
#include "worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct benchmark_suite_pthread {
	/** must be first member for allowing up-casting */
	struct benchmark_suite super;
	pthread_worker_t **workers;
	void *(*worker_proc)(task_t *task);
};

static int benchmark_suite_pthread_run(benchmark_suite_t *suite);
static int benchmark_suite_pthread_destroy(benchmark_suite_t *suite);

static struct benchmark_suite_vtable pthread_vtable = {
	.run = benchmark_suite_pthread_run,
	.destroy = benchmark_suite_pthread_destroy
};

static inline int
benchmark_suite_pthread_init(benchmark_suite_pthread_props_t *suite_props,
			     benchmark_suite_pthread_t *suite)
{
	pthread_worker_t **workers;
	long i;

	workers = malloc(suite->super.n_tasks * sizeof(*workers));
	if (!workers)
		return -1;

	suite->workers = workers;
	suite->worker_proc = suite_props->work_proc;

	return 0;
}

benchmark_suite_pthread_t *
benchmark_suite_pthread_create(benchmark_suite_pthread_props_t *suite_props)
{
	benchmark_suite_pthread_t *suite;
	int retval;

	suite = malloc(sizeof(*suite));
	if (!suite)
		return NULL;

	benchmark_suite_ctor(&suite->super, &suite_props->base);
	suite->super.vtable = &pthread_vtable;

	retval = benchmark_suite_pthread_init(suite_props, suite);
	if (retval) {
		free(suite);
		suite = NULL;
	}

	return suite;
}

static int benchmark_suite_pthread_run(benchmark_suite_t *suite)
{
	benchmark_suite_pthread_t *self = (benchmark_suite_pthread_t *) suite;
	pthread_worker_props_t worker_props = {
		.worker_routine = self->worker_proc
	};
	long i;
	int retval;

	for (i = 0L; i < self->super.n_tasks; i++) {
		worker_props.task = self->super.tasks[i];
		self->workers[i] = pthread_worker_create(&worker_props);
		if (!self->workers[i])
			goto worker_create;
	}

	// wait workers and destroy them
	for (i = 0L; i < self->super.n_tasks; i++) {
		retval = pthread_worker_destroy(self->workers[i]);
		if (retval)
			break;
	}

	return retval;
worker_create:
	for (; i > 0L; i--)
		pthread_worker_destroy(self->workers[i - 1]);
	return -1;
}

static int benchmark_suite_pthread_destroy(benchmark_suite_t *suite)
{
	benchmark_suite_pthread_t *self = (benchmark_suite_pthread_t *) suite;

	free(self->workers);

	return 0;
}
