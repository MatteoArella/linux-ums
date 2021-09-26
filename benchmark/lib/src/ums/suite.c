// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "ums/benchmark/ums_suite.h"
#include "../suite.h"
#include "scheduler.h"
#include "worker.h"

#include <stdlib.h>
#include <unistd.h>

struct benchmark_suite_ums {
	/** must be first member for allowing up-casting */
	struct benchmark_suite super;
	ums_completion_list_t *complist;
	ums_scheduler_t **schedulers;
	ums_worker_t **workers;
	long n_schedulers;
	void *(*worker_proc)(task_t *task);
};

static int benchmark_suite_ums_run(benchmark_suite_t *suite);
static int benchmark_suite_ums_destroy(benchmark_suite_t *suite);

static struct benchmark_suite_vtable ums_vtable = {
	.run = benchmark_suite_ums_run,
	.destroy = benchmark_suite_ums_destroy
};

static inline int
benchmark_suite_ums_init(benchmark_suite_ums_props_t *suite_props,
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

	workers = malloc(suite_props->base.n_tasks * sizeof(*workers));
	if (!workers)
		goto workers_alloc;

	suite->schedulers = schedulers;
	suite->workers = workers;
	suite->complist = suite_props->complist;
	suite->worker_proc = suite_props->work_proc;

	if (create_ums_completion_list(suite->complist))
		goto complist_create;

	sched_props.startup_info.completion_list = *suite->complist;
	sched_props.startup_info.ums_scheduler_entry_point =
			suite_props->sched_proc;
	sched_props.startup_info.scheduler_param = NULL;

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
benchmark_suite_ums_create(benchmark_suite_ums_props_t *suite_props)
{
	benchmark_suite_ums_t *suite;
	int retval;

	suite = malloc(sizeof(*suite));
	if (!suite)
		return NULL;

	benchmark_suite_ctor(&suite->super, &suite_props->base);
	suite->super.vtable = &ums_vtable;

	retval = benchmark_suite_ums_init(suite_props, suite);
	if (retval) {
		free(suite);
		suite = NULL;
	}

	return suite;
}

static int benchmark_suite_ums_run(benchmark_suite_t *suite)
{
	benchmark_suite_ums_t *self = (benchmark_suite_ums_t *) suite;
	ums_attr_t attr = {
		.completion_list = *self->complist,
		.pthread_attr = NULL
	};
	ums_worker_props_t worker_props = {
		.attr = &attr
	};
	long i;
	int retval;

	worker_props.worker_routine = self->worker_proc;

	for (i = 0L; i < self->super.n_tasks; i++) {
		worker_props.task = self->super.tasks[i];
		if (!(self->workers[i] =
					ums_worker_create(&worker_props)))
			goto worker_create;
	}

	// wait workers and destroy them
	for (i = 0L; i < self->super.n_tasks; i++) {
		retval = ums_worker_destroy(self->workers[i]);
		if (retval)
			break;
	}

	return retval;
worker_create:
	for (; i > 0; i--)
		ums_worker_destroy(self->workers[i - 1]);
	return -1;
}

static int benchmark_suite_ums_destroy(benchmark_suite_t *suite)
{
	benchmark_suite_ums_t *self = (benchmark_suite_ums_t *) suite;
	long i, n_schedulers = self->n_schedulers;
	int retval;

	for (i = 0L; i < n_schedulers; i++) {
		retval = ums_scheduler_destroy(self->schedulers[i]);
		if (retval)
			return retval;
	}

	free(self->schedulers);
	free(self->workers);

	return delete_ums_completion_list(self->complist);
}
