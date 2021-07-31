#include "pthread/model.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void *primality_test_pthread_worker_proc(task_t *task)
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

	return &task->result;
}
