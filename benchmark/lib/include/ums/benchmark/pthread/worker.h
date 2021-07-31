/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_PTHREAD_WORKER_H
#define UMS_BENCHMARK_PTHREAD_WORKER_H

#include <features.h>

#include "ums/benchmark/task.h"

__BEGIN_DECLS

typedef struct pthread_worker pthread_worker_t;

typedef struct pthread_worker_props {
	void *(*worker_routine)(task_t *task);
	task_t *task;
} pthread_worker_props_t;

pthread_worker_t *pthread_worker_create(pthread_worker_props_t *props);

int pthread_worker_destroy(pthread_worker_t *worker);

__END_DECLS

#endif /* UMS_BENCHMARK_PTHREAD_WORKER_H */
