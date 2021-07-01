#include "private.h"

typedef struct worker_proc_args_s {
	ums_completion_list_t completion_list;
	void *(*func)(void *);
	void *args;
} worker_proc_args_t;

static void *worker_wrapper_routine(void *args)
{
	worker_proc_args_t *worker_args = (worker_proc_args_t *) args;

	struct enter_ums_mode_args ums_args = {
		.flags = ENTER_UMS_WORK,
		.ums_complist = worker_args->completion_list
	};
	int retval;

	// enter ums mode and deschedule worker thread (suspend here)
	if (enter_ums_mode(&ums_args))
		return NULL;

	// worker thread is now active for scheduling
	return worker_args->func(worker_args->args);
}

int ums_pthread_create(pthread_t *thread, ums_attr_t *ums_attr,
			     void *(*func)(void *), void *args)
{
	worker_proc_args_t ums_args = {
		.completion_list = ums_attr->completion_list,
		.func = func,
		.args = args
	};
	pthread_attr_t default_pthread_attr;
	pthread_attr_t *pthread_attr = &default_pthread_attr;

	if (ums_attr->pthread_attr)
		pthread_attr = ums_attr->pthread_attr;
	else
		pthread_attr_init(pthread_attr);

	// create detached
	pthread_attr_setdetachstate(pthread_attr,
				    PTHREAD_CREATE_DETACHED);

	return pthread_create(thread,
			      pthread_attr,
			      worker_wrapper_routine,
			      &ums_args);
}

int ums_thread_yield(void *scheduler_param)
{
	errno = ENOSYS;
	return -1;
}
