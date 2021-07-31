/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_PTHREAD_SUITE_CORE_H
#define UMS_BENCHMARK_PTHREAD_SUITE_CORE_H

#include "ums/benchmark/suite.h"

typedef struct benchmark_suite_pthread benchmark_suite_pthread_t;

benchmark_suite_pthread_t *
benchmark_suite_pthread_create(benchmark_suite_props_t *suite_props);

int benchmark_suite_pthread_run(benchmark_suite_pthread_t *suite);

int benchmark_suite_pthread_destroy(benchmark_suite_pthread_t *suite);

#endif /* UMS_BENCHMARK_PTHREAD_SUITE_CORE_H */
