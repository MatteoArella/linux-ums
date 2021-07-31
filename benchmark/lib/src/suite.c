// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "timer.h"
#include "ums/benchmark/suite.h"
#include "ums/suite.h"
#include "pthread/suite.h"

#include <stdlib.h>

struct benchmark_suite {
	benchmark_suite_type_t type;
	struct timer timer;
	union {
		benchmark_suite_ums_t *ums_suite;
		benchmark_suite_pthread_t *pthread_suite;
	};
};

benchmark_suite_t *
benchmark_suite_create(benchmark_suite_props_t *suite_props)
{
	struct benchmark_suite *suite;
	int retval;

	suite = malloc(sizeof(*suite));
	if (!suite)
		return NULL;

	suite->type = suite_props->type;

	switch (suite_props->type) {
	case BENCHMARK_SUITE_UMS:
		suite->ums_suite = benchmark_suite_ums_create(suite_props);
		retval = suite->ums_suite == NULL;
		break;
	case BENCHMARK_SUITE_PTHREAD:
		suite->pthread_suite =
				benchmark_suite_pthread_create(suite_props);
		retval = suite->pthread_suite == NULL;
		break;
	default:
		retval = -1;
		break;
	}

	if (!retval)
		return suite;

	free(suite);
	return NULL;
}

int benchmark_suite_run(benchmark_suite_t *suite)
{
	int retval;

	timer_start(&suite->timer);
	switch (suite->type) {
	case BENCHMARK_SUITE_UMS:
		retval = benchmark_suite_ums_run(suite->ums_suite);
		break;
	case BENCHMARK_SUITE_PTHREAD:
		retval = benchmark_suite_pthread_run(suite->pthread_suite);
		break;
	default:
		retval = -1;
		break;
	}
	timer_stop(&suite->timer);

	return retval;
}

struct timer *benchmark_suite_elapsed_time(benchmark_suite_t *suite)
{
	return &suite->timer;
}

int benchmark_suite_destroy(benchmark_suite_t *suite)
{
	int retval;

	switch (suite->type) {
	case BENCHMARK_SUITE_UMS:
		retval = benchmark_suite_ums_destroy(suite->ums_suite);
		break;
	case BENCHMARK_SUITE_PTHREAD:
		retval = benchmark_suite_pthread_destroy(suite->pthread_suite);
		break;
	default:
		return -1;
	}

	free(suite);

	return retval;
}
