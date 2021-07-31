/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_PTHREAD_MODEL_H
#define UMS_BENCHMARK_PTHREAD_MODEL_H

#include "task.h"

void *primality_test_pthread_worker_proc(task_t *task);

#endif /* UMS_BENCHMARK_PTHREAD_MODEL_H */
