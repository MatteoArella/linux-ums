#define _GNU_SOURCE

#include <ums.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

ums_completion_list_t comp_list;

/* TODO: */
static void sched_entry_proc(ums_reason_t reason,
			     ums_activation_t *activation,
			     void *args)
{
	switch (reason) {
	case UMS_SCHEDULER_STARTUP:
		printf("sched_entry_proc: UMS_SCHEDULER_STARTUP\n");
		break;
	case UMS_SCHEDULER_THREAD_BLOCKED:
		break;
	case UMS_SCHEDULER_THREAD_YIELD:
		break;
	default:
		break;
	}
}

static void *sched_pthread_proc(void *arg)
{
	ums_scheduler_startup_info_t sched_info;

	sched_info.completion_list = comp_list;
	sched_info.scheduler_param = NULL;
	sched_info.ums_scheduler_entry_point = sched_entry_proc;

	printf("starting scheduler thread\n");

	enter_ums_scheduling_mode(&sched_info);

	return NULL;
}

int initialize_ums_scheduling(pthread_t *sched_threads,
			     long nthreads)
{
	pthread_attr_t attr;
	cpu_set_t cpus;
	long i;

	if (pthread_attr_init(&attr) != 0)
		goto err;

	if (create_ums_completion_list(&comp_list) != 0)
		goto err;

	for (i = 0L; i < nthreads; i++) {
		CPU_ZERO(&cpus);
		CPU_SET(i, &cpus);
		if (pthread_attr_setaffinity_np(&attr,
						sizeof(cpu_set_t),
						&cpus) != 0)
			goto comp_list_create;
		if (pthread_create(sched_threads + i,
				  &attr,
				  sched_pthread_proc,
				  NULL) != 0)
			goto pthread_create;
	}
	return 0;

pthread_create:
	for (; i > 0; i--) {
		/* TODO: check ums pthread scheduler cancellation points */
		pthread_cancel(sched_threads[i - 1]);
		pthread_join(sched_threads[i - 1], NULL);
	}
comp_list_create:
	delete_ums_completion_list(&comp_list);
err:
	return -1;
}

int release_ums_scheduling(pthread_t *sched_threads,
			  long nthreads)
{
	long i;

	for (i = 0L; i < nthreads; i++) {
		pthread_join(sched_threads[i], NULL);
	}

	delete_ums_completion_list(&comp_list);

	return 0;
}

int create_ums_worker_thread(pthread_t *thread, void *(*func)(void *),
			    void *arg)
{
	ums_context_t ctx;
	ums_attr_t attr;

	if (create_ums_thread_context(&ctx) != 0)
		goto err;

	attr.completion_list = comp_list;
	attr.ums_context = ctx;
	attr.pthread_attr = NULL;

	if (ums_pthread_create(thread, &attr, func, arg))
		goto create_ums_ctx;

create_ums_ctx:
	delete_ums_thread_context(&ctx);
err:
	return -1;
}

/* TODO: */
static void *worker_pthread_proc(void *arg)
{
	return NULL;
}

int main(int argc, char** argv)
{
	long 			nproc = sysconf(_SC_NPROCESSORS_ONLN);
	pthread_t 		sched_threads[nproc];

	if (initialize_ums_scheduling(sched_threads, nproc) != 0) {
		perror("initialize_ums_scheduling");
		return 1;
	}

	/* TODO: create tasks and ums worker threads */
	pthread_t worker;

	if (create_ums_worker_thread(&worker, worker_pthread_proc, NULL) != 0) {
		perror("create_ums_worker_thread");
	}

	if (release_ums_scheduling(sched_threads, nproc) != 0) {
		perror("release_ums_scheduling");
		return 1;
	}

	return 0;
}