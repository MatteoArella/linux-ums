// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "ums/benchmark/ums/scheduler.h"
#include "ums/benchmark/ums/worker.h"
#include "suite.h"

#include <stdlib.h>
#include <unistd.h>

struct benchmark_suite_ums {
	benchmark_suite_props_t props;
	ums_completion_list_t *complist;
	long n_schedulers;
	ums_scheduler_t **schedulers;
	ums_worker_t **workers;
};

static inline int
benchmark_suite_ums_init(benchmark_suite_props_t *suite_props,
			 benchmark_suite_ums_t *suite)
{
	ums_scheduler_t **schedulers;
	ums_worker_t **workers;
	cpu_set_t cpus;
	long i, n_schedulers = sysconf(_SC_NPROCESSORS_ONLN);
	ums_scheduler_props_t sched_props;

	suite->n_schedulers = n_schedulers;

	schedulers = malloc(n_schedulers * sizeof(*schedulers));
	if (!schedulers)
		return -1;

	workers = malloc(suite_props->n_tasks * sizeof(*workers));
	if (!workers)
		goto workers_alloc;

	suite->schedulers = schedulers;
	suite->workers = workers;
	suite->props = *suite_props;

	if (create_ums_completion_list(suite_props->ums_props.complist))
		goto complist_create;

	suite->complist = suite_props->ums_props.complist;
	sched_props.startup_info.completion_list = *suite->complist;
	sched_props.startup_info.ums_scheduler_entry_point =
			suite_props->ums_props.scheduler_props.sched_proc;
	sched_props.startup_info.scheduler_param = NULL;
	sched_props.cleanup_routine =
			suite_props->ums_props.scheduler_props.cleanup_routine;

	for (i = 0L; i < n_schedulers; i++) {
		CPU_ZERO(&cpus);
		CPU_SET(i, &cpus);
		sched_props.cpu_set = cpus;
		suite->schedulers[i] = ums_scheduler_create(&sched_props);
		if (!suite->schedulers[i])
			goto scheduler_create;
	}

	return 0;

scheduler_create:
	for (; i > 0; i--)
		ums_scheduler_destroy(suite->schedulers[i - 1]);
	delete_ums_completion_list(suite->complist);
complist_create:
	free(workers);
workers_alloc:
	free(schedulers);
	return -1;
}

benchmark_suite_ums_t *
benchmark_suite_ums_create(benchmark_suite_props_t *suite_props)
{
	benchmark_suite_ums_t *suite;
	int retval;

	suite = malloc(sizeof(*suite));
	if (!suite)
		return NULL;

	retval = benchmark_suite_ums_init(suite_props, suite);
	if (retval) {
		free(suite);
		suite = NULL;
	}

	return suite;
}

int benchmark_suite_ums_run(benchmark_suite_ums_t *suite)
{
	long i;
	ums_attr_t attr = {
		.completion_list = *suite->complist,
		.pthread_attr = NULL
	};
	ums_worker_props_t worker_props = {
		.attr = &attr
	};
	int retval;

	worker_props.worker_routine =
			suite->props.ums_props.worker_props.work_proc;

	for (i = 0L; i < suite->props.n_tasks; i++) {
		worker_props.task = suite->props.tasks[i];
		if (!(suite->workers[i] =
					ums_worker_create(&worker_props)))
			goto worker_create;
	}

	// wait workers and destroy them
	for (i = 0L; i < suite->props.n_tasks; i++) {
		retval = ums_worker_destroy(suite->workers[i]);
		if (retval)
			break;
	}

	return retval;
worker_create:
	for (; i > 0; i--)
		ums_worker_destroy(suite->workers[i - 1]);
	return -1;
}

int benchmark_suite_ums_destroy(benchmark_suite_ums_t *suite)
{
	long i, n_schedulers = suite->n_schedulers;
	int retval;

	for (i = 0L; i < n_schedulers; i++) {
		retval = ums_scheduler_destroy(suite->schedulers[i]);
		if (retval)
			return retval;
	}

	free(suite->schedulers);
	free(suite->workers);

	return delete_ums_completion_list(suite->complist);
}
