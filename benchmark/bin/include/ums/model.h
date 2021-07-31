/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_UMS_MODEL_H
#define UMS_BENCHMARK_UMS_MODEL_H

#include "task.h"

#include <ums.h>

extern ums_completion_list_t comp_list;

void sched_entry_proc(ums_reason_t reason,
		      ums_activation_t *activation,
		      void *args);

void *primality_test_ums_worker_proc(task_t *task);

#endif /* UMS_BENCHMARK_MODEL_H */
