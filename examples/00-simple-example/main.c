// SPDX-License-Identifier: AGPL-3.0-only

#define _GNU_SOURCE

#include <ums.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#include "list.h"

struct context_list_node {
	ums_context_t context;
	struct list_head list;
};

struct ums_sched_rq {
	struct list_head head;
};

ums_completion_list_t comp_list;
__thread struct ums_sched_rq rq;

static struct context_list_node *get_next_context(void)
{
	ums_context_t context;
	struct context_list_node *node;

	if (!list_empty(&rq.head)) {
		node = list_first_entry(&rq.head,
					struct context_list_node,
					list);
		list_del(&node->list);
		return node;
	}

	while (dequeue_ums_completion_list_items(comp_list, &context)) {
		if (errno == EINTR)
			continue;
		else
			return NULL;
	}

	node = malloc(sizeof(*node));
	if (!node)
		return NULL;
	node->context = context;

	list_add_tail(&node->list, &rq.head);

	while ((context = get_next_ums_list_item(context)) > 0) {
		node = malloc(sizeof(*node));
		if (!node)
			return NULL;
		node->context = context;

		list_add_tail(&node->list, &rq.head);
	}

	node = list_first_entry(&rq.head,
				struct context_list_node,
				list);
	list_del(&node->list);

	return node;
}

static inline void execute_next_context(void)
{
	struct context_list_node *node;
	ums_context_t context;

	node = get_next_context();
	if (!node) {
		perror("get_next_context");
		return;
	}

	if (execute_ums_thread(node->context))
		perror("execute_ums_thread");

	free(node);
}

static void sched_entry_proc(ums_reason_t reason,
			     ums_activation_t *activation,
			     void *args)
{
	ums_context_t context;
	long worker_result;

	switch (reason) {
	case UMS_SCHEDULER_STARTUP:
		execute_next_context();
		break;
	case UMS_SCHEDULER_THREAD_YIELD:
		context = activation->context;
		worker_result = *((long *) args);

		printf("worker %d yielded with value %ld\n",
		       context,
		       worker_result);
		fflush(stdout);

		free(args);

		execute_next_context();
		break;
	case UMS_SCHEDULER_THREAD_END:
		execute_next_context();
		break;
	default:
		break;
	}
}

static void *sched_pthread_proc(void *arg)
{
	ums_scheduler_startup_info_t sched_info;

	sched_info.completion_list = comp_list;
	sched_info.scheduler_param = arg;
	sched_info.ums_scheduler_entry_point = sched_entry_proc;

	INIT_LIST_HEAD(&rq.head);

	if (enter_ums_scheduling_mode(&sched_info))
		perror("enter_ums_scheduling_mode");

	return NULL;
}

int initialize_ums_scheduling(pthread_t *sched_threads,
			     long nthreads)
{
	pthread_attr_t attr;
	cpu_set_t cpus;
	long i;

	if (pthread_attr_init(&attr))
		return -1;

	if (create_ums_completion_list(&comp_list))
		return -1;

	for (i = 0L; i < nthreads; i++) {
		CPU_ZERO(&cpus);
		CPU_SET(i, &cpus);
		if (pthread_attr_setaffinity_np(&attr,
						sizeof(cpu_set_t),
						&cpus))
			goto out;
		if (pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED))
			goto out;

		if (pthread_create(sched_threads + i,
				   &attr,
				   sched_pthread_proc,
				   NULL))
			goto out;
	}

	return 0;
out:
	delete_ums_completion_list(&comp_list);
	return -1;
}

int release_ums_scheduling(pthread_t *sched_threads,
			   long nthreads)
{
	return delete_ums_completion_list(&comp_list);
}

int create_ums_worker_thread(pthread_t *thread, void *(*func)(void *),
			    void *arg)
{
	ums_attr_t attr;

	attr.completion_list = comp_list;
	attr.pthread_attr = NULL;

	return ums_pthread_create(thread, &attr, func, arg);
}

static void *worker_pthread_proc(void *arg)
{
	long *result;

	result = malloc(sizeof(*result));
	if (!result)
		return NULL;

	*result = (long) (intptr_t) arg;

	if (ums_thread_yield(result))
		perror("ums_thread_yield");

	return NULL;
}

int main(int argc, char **argv)
{
	long			nproc = sysconf(_SC_NPROCESSORS_ONLN);
	pthread_t		sched_threads[nproc];
	long			nworkers = 24L * nproc;
	pthread_t		workers[nworkers];
	long			i;

	if (initialize_ums_scheduling(sched_threads, nproc)) {
		perror("initialize_ums_scheduling");
		return 1;
	}

	for (i = 0L; i < nworkers; i++) {
		if (create_ums_worker_thread(workers + i,
					     worker_pthread_proc,
					     (void *) i))
			perror("create_ums_worker_thread");
	}

	for (i = 0L; i < nworkers; i++)
		pthread_join(workers[i], NULL);

	if (release_ums_scheduling(sched_threads, nproc)) {
		perror("release_ums_scheduling");
		return 1;
	}

	return 0;
}
