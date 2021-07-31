// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "ums/benchmark/pthread/worker.h"
#include "suite.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct benchmark_suite_pthread {
	benchmark_suite_props_t props;
	pthread_worker_t **workers;
};

static inline int
benchmark_suite_pthread_init(benchmark_suite_props_t *suite_props,
			     benchmark_suite_pthread_t *suite)
{
	pthread_worker_t **workers;
	long i;

	workers = malloc(suite_props->n_tasks * sizeof(*workers));
	if (!workers)
		return -1;

	suite->workers = workers;
	suite->props = *suite_props;

	return 0;
}

benchmark_suite_pthread_t *
benchmark_suite_pthread_create(benchmark_suite_props_t *suite_props)
{
	benchmark_suite_pthread_t *suite;
	int retval;

	suite = malloc(sizeof(*suite));
	if (!suite)
		return NULL;

	retval = benchmark_suite_pthread_init(suite_props, suite);
	if (retval) {
		free(suite);
		suite = NULL;
	}

	return suite;
}

int benchmark_suite_pthread_run(benchmark_suite_pthread_t *suite)
{
	long i;
	pthread_worker_props_t worker_props = {
		.worker_routine = suite->props.pthread_props.work_proc
	};
	int retval;

	for (i = 0L; i < suite->props.n_tasks; i++) {
		worker_props.task = suite->props.tasks[i];
		suite->workers[i] = pthread_worker_create(&worker_props);
		if (!suite->workers[i])
			goto worker_create;
	}

	// wait workers and destroy them
	for (i = 0L; i < suite->props.n_tasks; i++) {
		retval = pthread_worker_destroy(suite->workers[i]);
		if (retval)
			break;
	}

	return retval;
worker_create:
	for (; i > 0L; i--)
		pthread_worker_destroy(suite->workers[i - 1]);
	return -1;
}

int benchmark_suite_pthread_destroy(benchmark_suite_pthread_t *suite)
{
	free(suite->workers);

	return 0;
}
