// SPDX-License-Identifier: AGPL-3.0-only

#include "suite.h"

#include <stdlib.h>

void benchmark_suite_ctor(benchmark_suite_t *suite,
			  benchmark_suite_props_t *props)
{
	suite->tasks = props->tasks;
	suite->n_tasks = props->n_tasks;
}

int benchmark_suite_run(benchmark_suite_t *suite)
{
	int retval;

	timer_start(&suite->timer);
	retval = suite->vtable->run(suite);
	timer_stop(&suite->timer);

	return retval;
}

struct timer *benchmark_suite_elapsed_time(benchmark_suite_t *suite)
{
	return &suite->timer;
}

int benchmark_suite_destroy(benchmark_suite_t *suite)
{
	return suite->vtable->destroy(suite);
}
