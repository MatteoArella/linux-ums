/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_UMS_SUITE_CORE_H
#define UMS_BENCHMARK_UMS_SUITE_CORE_H

#include "ums/benchmark/suite.h"

typedef struct benchmark_suite_ums benchmark_suite_ums_t;

benchmark_suite_ums_t *
benchmark_suite_ums_create(benchmark_suite_props_t *suite_props);

int benchmark_suite_ums_run(benchmark_suite_ums_t *suite);

int benchmark_suite_ums_destroy(benchmark_suite_ums_t *suite);

#endif /* UMS_BENCHMARK_UMS_SUITE_CORE_H */
