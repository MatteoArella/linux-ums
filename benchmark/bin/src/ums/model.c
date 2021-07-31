#include "list.h"
#include "ums/model.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

struct context_list_node {
	ums_context_t context;
	struct list_head list;
};

struct ums_sched_rq {
	struct list_head head;
};

__thread struct ums_sched_rq rq;
ums_completion_list_t comp_list;

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

void sched_entry_proc(ums_reason_t reason,
		      ums_activation_t *activation,
		      void *args)
{
	switch (reason) {
	case UMS_SCHEDULER_STARTUP:
		INIT_LIST_HEAD(&rq.head);
		execute_next_context();
		break;
	case UMS_SCHEDULER_THREAD_YIELD:
		execute_next_context();
		break;
	case UMS_SCHEDULER_THREAD_END:
		execute_next_context();
		break;
	default:
		break;
	}
}

void *primality_test_ums_worker_proc(task_t *task)
{
	unsigned long i, num = task->input;

	task->result = 0;

	// check if num is prime
	for (i = 2UL; i <= num / 2UL; i++) {
		if (num % i == 0) {
			task->result = 1;
			break;
		}
	}

	if (ums_thread_yield(&task->result))
		perror("ums_thread_yield");

	return NULL;
}
