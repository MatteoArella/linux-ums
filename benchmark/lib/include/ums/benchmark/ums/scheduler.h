/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_UMS_SCHEDULER_H
#define UMS_BENCHMARK_UMS_SCHEDULER_H

#include <ums.h>
#include <features.h>

__BEGIN_DECLS

typedef struct ums_scheduler ums_scheduler_t;

typedef struct ums_scheduler_props_s {
	cpu_set_t cpu_set;
	void (*cleanup_routine)(void *args);
	ums_scheduler_startup_info_t startup_info;
} ums_scheduler_props_t;

ums_scheduler_t *ums_scheduler_create(ums_scheduler_props_t *props);

int ums_scheduler_destroy(ums_scheduler_t *scheduler);

__END_DECLS

#endif /* UMS_BENCHMARK_UMS_SCHEDULER_H */
