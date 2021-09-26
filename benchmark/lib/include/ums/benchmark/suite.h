/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_SUITE_H
#define UMS_BENCHMARK_SUITE_H

#include "task.h"
#include "timer.h"

#include <features.h>

__BEGIN_DECLS

typedef struct benchmark_suite benchmark_suite_t;

typedef struct benchmark_suite_props {
	task_t **tasks;
	unsigned long n_tasks;
} benchmark_suite_props_t;

int benchmark_suite_run(benchmark_suite_t *suite);

struct timer *benchmark_suite_elapsed_time(benchmark_suite_t *suite);

int benchmark_suite_destroy(benchmark_suite_t *benchmark_suite);

__END_DECLS

#endif /* UMS_BENCHMARK_SUITE_H */
