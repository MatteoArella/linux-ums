/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_PTHREAD_SUITE_H
#define UMS_BENCHMARK_PTHREAD_SUITE_H

#include <features.h>

#include "ums/benchmark/task.h"

__BEGIN_DECLS

typedef struct benchmark_suite_pthread_props {
	void *(*work_proc)(task_t *task);
} benchmark_suite_pthread_props_t;

__END_DECLS

#endif /* UMS_BENCHMARK_PTHREAD_SUITE_H */
