/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_TIMER_CORE_H
#define UMS_BENCHMARK_TIMER_CORE_H

#include "ums/benchmark/timer.h"

#include <features.h>
#include <time.h>

__BEGIN_DECLS

struct timer {
	struct timespec begin;
	struct timespec end;
	struct timespec elapsed;
};

void timer_start(struct timer *t);
void timer_stop(struct timer *t);

__END_DECLS

#endif /* UMS_BENCHMARK_TIMER_CORE_H */
