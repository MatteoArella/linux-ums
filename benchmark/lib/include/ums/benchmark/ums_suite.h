/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_UMS_SUITE_H
#define UMS_BENCHMARK_UMS_SUITE_H

#include "suite.h"
#include "task.h"

#include <ums.h>
#include <features.h>

__BEGIN_DECLS

typedef struct benchmark_suite_ums_props {
	benchmark_suite_props_t base;
	ums_completion_list_t *complist;
	void (*sched_proc)(ums_reason_t, ums_activation_t *, void *);
	void *(*work_proc)(task_t *task);
} benchmark_suite_ums_props_t;

typedef struct benchmark_suite_ums benchmark_suite_ums_t;

benchmark_suite_ums_t *
benchmark_suite_ums_create(benchmark_suite_ums_props_t *suite_props);

__END_DECLS

#endif /* UMS_BENCHMARK_UMS_SUITE_H */
