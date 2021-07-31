// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include "ums/benchmark/ums/scheduler.h"

#include <stdlib.h>
#include <pthread.h>

struct ums_scheduler {
	pthread_t thread;
	ums_scheduler_startup_info_t startup_info;
	void (*cleanup_routine)(void *args);
};

void default_cleanup_routine(void *args) {}

static void *sched_pthread_proc(void *arg)
{
	struct ums_scheduler *scheduler = arg;

	pthread_cleanup_push(scheduler->cleanup_routine,
			     NULL);

	enter_ums_scheduling_mode(&scheduler->startup_info);

	pthread_cleanup_pop(1);

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
	scheduler->cleanup_routine = props->cleanup_routine ?
			props->cleanup_routine : default_cleanup_routine;

	if (pthread_attr_init(&attr))
		goto out;

	if (pthread_attr_setaffinity_np(&attr,
					sizeof(cpu_set_t),
					&props->cpu_set))
		goto out;

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
