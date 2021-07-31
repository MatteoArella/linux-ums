/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_SUITE_H
#define UMS_BENCHMARK_SUITE_H

#include "ums/benchmark/task.h"
#include "ums/benchmark/ums/suite.h"
#include "ums/benchmark/pthread/suite.h"
#include "ums/benchmark/timer.h"

#include <features.h>

__BEGIN_DECLS

typedef struct benchmark_suite benchmark_suite_t;

typedef enum benchmark_suite_type {
	BENCHMARK_SUITE_UMS = 0,
#define BENCHMARK_SUITE_UMS BENCHMARK_SUITE_UMS

	BENCHMARK_SUITE_PTHREAD
#define BENCHMARK_SUITE_PTHREAD BENCHMARK_SUITE_PTHREAD
} benchmark_suite_type_t;

typedef struct benchmark_suite_props {
	benchmark_suite_type_t type;
	task_t **tasks;
	unsigned long n_tasks;
	union {
		benchmark_suite_ums_props_t ums_props;
		benchmark_suite_pthread_props_t pthread_props;
	};
} benchmark_suite_props_t;

benchmark_suite_t *
benchmark_suite_create(benchmark_suite_props_t *suite_props);

int benchmark_suite_run(benchmark_suite_t *suite);

struct timer *benchmark_suite_elapsed_time(benchmark_suite_t *suite);

int benchmark_suite_destroy(benchmark_suite_t *benchmark_suite);

__END_DECLS

#endif /* UMS_BENCHMARK_SUITE_H */
