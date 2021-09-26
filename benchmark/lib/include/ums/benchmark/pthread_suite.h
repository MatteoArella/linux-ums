/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_PTHREAD_SUITE_H
#define UMS_BENCHMARK_PTHREAD_SUITE_H

#include "suite.h"
#include "task.h"

#include <features.h>

__BEGIN_DECLS

typedef struct benchmark_suite_pthread_props {
	benchmark_suite_props_t base;
	void *(*work_proc)(task_t *task);
} benchmark_suite_pthread_props_t;

typedef struct benchmark_suite_pthread benchmark_suite_pthread_t;

benchmark_suite_pthread_t *
benchmark_suite_pthread_create(benchmark_suite_pthread_props_t *suite_props);

__END_DECLS

#endif /* UMS_BENCHMARK_PTHREAD_SUITE_H */
