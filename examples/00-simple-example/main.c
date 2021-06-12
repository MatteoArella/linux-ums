#define _GNU_SOURCE

#include <ums.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#include "../util/list.h"

struct context_list_node {
	ums_context_t context;
	struct list_head list;
};

struct ums_runqueue {
	pthread_mutex_t lock;
	struct list_head head;
};

ums_completion_list_t comp_list;
struct ums_runqueue rq;

static int enqueue_available_contexts(void)
{
	ums_context_t context;
	struct context_list_node *node;

	pthread_mutex_lock(&rq.lock);
	if (!list_empty(&rq.head)) {
		pthread_mutex_unlock(&rq.lock);
		return 0;
	}
	pthread_mutex_unlock(&rq.lock);

	while (dequeue_ums_completion_list_items(comp_list, &context)) {
		if (errno == EINTR)
			continue;
		else
			return -1;
	}

	node = calloc(1, sizeof(*node));
	if (!node)
		return -1;
	node->context = context;

	pthread_mutex_lock(&rq.lock);
	list_add_tail(&node->list, &rq.head);
	pthread_mutex_unlock(&rq.lock);

	while ((context = get_next_ums_list_item(context)) != -1) {
		node = calloc(1, sizeof(*node));
		if (!node)
			return -1;
		node->context = context;

		pthread_mutex_lock(&rq.lock);
		list_add_tail(&node->list, &rq.head);
		pthread_mutex_unlock(&rq.lock);
	}

	return 0;
}

/* TODO: */
static void sched_entry_proc(ums_reason_t reason,
			     ums_activation_t *activation,
			     void *args)
{
	ums_context_t context;
	struct context_list_node *node;
	long sched_id = (long) (intptr_t) args;

	switch (reason) {
	case UMS_SCHEDULER_STARTUP:
		if (enqueue_available_contexts()) {
			perror("enqueue_available_contexts");
			return;
		}

		pthread_mutex_lock(&rq.lock);
		node = list_first_entry(&rq.head,
					struct context_list_node,
					list);
		list_del(&node->list);
		pthread_mutex_unlock(&rq.lock);

		printf("[sched-%ld] using context %d\n",
			sched_id,
			node->context);
		fflush(stdout);
		free(node);

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
	sched_info.scheduler_param = arg;
	sched_info.ums_scheduler_entry_point = sched_entry_proc;

	enter_ums_scheduling_mode(&sched_info);

	return NULL;
}

int initialize_ums_scheduling(pthread_t *sched_threads,
			     long nthreads)
{
	pthread_attr_t attr;
	cpu_set_t cpus;
	long i;

	INIT_LIST_HEAD(&rq.head);
	pthread_mutex_init(&rq.lock, NULL);

	if (pthread_attr_init(&attr))
		goto err;

	if (create_ums_completion_list(&comp_list))
		goto err;

	for (i = 0L; i < nthreads; i++) {
		CPU_ZERO(&cpus);
		CPU_SET(i, &cpus);
		if (pthread_attr_setaffinity_np(&attr,
						sizeof(cpu_set_t),
						&cpus))
			goto comp_list_create;

		if (pthread_create(sched_threads + i,
				  &attr,
				  sched_pthread_proc,
				  (void *) i))
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

	for (i = 0L; i < nthreads; i++)
		pthread_join(sched_threads[i], NULL);

	delete_ums_completion_list(&comp_list);

	pthread_mutex_destroy(&rq.lock);

	return 0;
}

int create_ums_worker_thread(pthread_t *thread, void *(*func)(void *),
			    void *arg)
{
	ums_attr_t attr;

	attr.completion_list = comp_list;
	attr.pthread_attr = NULL;

	return ums_pthread_create(thread, &attr, func, arg);
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

	if (initialize_ums_scheduling(sched_threads, nproc)) {
		perror("initialize_ums_scheduling");
		return 1;
	}

	/* TODO: create tasks and ums worker threads */
	int nworkers = 24 * nproc;
	pthread_t workers[nworkers];
	int i;
	for (i = 0; i < nworkers; i++)
	if (create_ums_worker_thread(workers + i, worker_pthread_proc, NULL))
		perror("create_ums_worker_thread");

	if (release_ums_scheduling(sched_threads, nproc)) {
		perror("release_ums_scheduling");
		return 1;
	}

	return 0;
}
