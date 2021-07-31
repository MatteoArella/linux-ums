/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_UMS_SUITE_H
#define UMS_BENCHMARK_UMS_SUITE_H

#include "ums/benchmark/task.h"

#include <ums.h>
#include <features.h>

__BEGIN_DECLS

typedef struct benchmark_suite_ums_scheduler_props {
	void (*cleanup_routine)(void *args);
	void (*sched_proc)(ums_reason_t,
			   ums_activation_t *,
			   void *);
} benchmark_suite_ums_scheduler_props_t;

typedef struct benchmark_suite_ums_worker_props {
	void *(*work_proc)(task_t *task);
} benchmark_suite_ums_worker_props_t;

typedef struct benchmark_suite_ums_props {
	ums_completion_list_t *complist;
	benchmark_suite_ums_scheduler_props_t scheduler_props;
	benchmark_suite_ums_worker_props_t worker_props;
} benchmark_suite_ums_props_t;

__END_DECLS

#endif /* UMS_BENCHMARK_UMS_SUITE_H */
