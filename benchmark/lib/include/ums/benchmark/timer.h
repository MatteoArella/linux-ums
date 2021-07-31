/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_BENCHMARK_TIMER_H
#define UMS_BENCHMARK_TIMER_H

#include <features.h>

__BEGIN_DECLS

struct timer;

unsigned long int get_nanoseconds(struct timer *t);

unsigned long int get_microseconds(struct timer *t);

unsigned long int get_milliseconds(struct timer *t);

unsigned long int get_seconds(struct timer *t);

__END_DECLS

#endif /* UMS_BENCHMARK_TIMER_H */
