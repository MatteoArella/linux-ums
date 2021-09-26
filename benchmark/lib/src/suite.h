/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_SUITE_CORE_H
#define UMS_BENCHMARK_SUITE_CORE_H

#include "ums/benchmark/suite.h"
#include "timer.h"

#include <features.h>

__BEGIN_DECLS

struct benchmark_suite_vtable {
	int (*run)(benchmark_suite_t *suite);
	int (*destroy)(benchmark_suite_t *suite);
};

struct benchmark_suite {
	struct benchmark_suite_vtable *vtable;
	task_t **tasks;
	unsigned long n_tasks;
	struct timer timer;
};

void benchmark_suite_ctor(benchmark_suite_t *suite,
			  benchmark_suite_props_t *props);

__END_DECLS

#endif /* UMS_BENCHMARK_SUITE_CORE_H */
