/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_UMS_WORKER_H
#define UMS_BENCHMARK_UMS_WORKER_H

#include "ums/benchmark/task.h"

#include <ums.h>
#include <features.h>

__BEGIN_DECLS

typedef struct ums_worker ums_worker_t;

typedef struct ums_worker_props {
	ums_attr_t *attr;
	void *(*worker_routine)(task_t *task);
	task_t *task;
} ums_worker_props_t;

ums_worker_t *ums_worker_create(ums_worker_props_t *props);

int ums_worker_destroy(ums_worker_t *worker);

__END_DECLS

#endif /* UMS_BENCHMARK_UMS_WORKER_H */
