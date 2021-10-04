// SPDX-License-Identifier: AGPL-3.0-only

#include "../global.h"

#include "scheduler.h"

#include <stdlib.h>
#include <pthread.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

struct ums_scheduler {
#if !HAVE_DECL_PTHREAD_ATTR_SETAFFINITY_NP && defined(HAVE_SCHED_SETAFFINITY)
	cpu_set_t cpu_set;
#endif
	pthread_t thread;
	ums_scheduler_startup_info_t startup_info;
};

static void *sched_pthread_proc(void *arg)
{
	struct ums_scheduler *scheduler = arg;

#if !HAVE_DECL_PTHREAD_ATTR_SETAFFINITY_NP && defined(HAVE_SCHED_SETAFFINITY)
	(void) sched_setaffinity(0, sizeof(cpu_set_t), &scheduler->cpu_set);
#endif

	enter_ums_scheduling_mode(&scheduler->startup_info);

	return NULL;
}

ums_scheduler_t *ums_scheduler_create(ums_scheduler_props_t *props)
{
	struct ums_scheduler *scheduler;
	pthread_attr_t attr;

	scheduler = malloc(sizeof(*scheduler));
	if (!scheduler)
		return NULL;

	scheduler->startup_info = props->startup_info;

	if (pthread_attr_init(&attr))
		goto out;

#if HAVE_DECL_PTHREAD_ATTR_SETAFFINITY_NP
	if (pthread_attr_setaffinity_np(&attr,
					sizeof(cpu_set_t),
					&props->cpu_set))
		goto out;
#else /* !HAVE_DECL_PTHREAD_ATTR_SETAFFINITY_NP */
#ifdef HAVE_SCHED_SETAFFINITY
	scheduler->cpu_set = props->cpu_set;
#endif
#endif /* !HAVE_DECL_PTHREAD_ATTR_SETAFFINITY_NP */

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
		goto out;

	if (pthread_create(&scheduler->thread,
			   &attr,
			   sched_pthread_proc,
			   scheduler))
		goto out;

	return scheduler;
out:
	free(scheduler);
	return NULL;
}

int ums_scheduler_destroy(ums_scheduler_t *scheduler)
{
	free(scheduler);
	return 0;
}
