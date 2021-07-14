#include "private.h"

typedef struct worker_proc_args_s {
	ums_completion_list_t completion_list;
	void *(*func)(void *);
	void *args;
} worker_proc_args_t;

static pthread_key_t worker_key;
static pthread_once_t worker_key_once = PTHREAD_ONCE_INIT;

static inline int exit_ums_mode(void)
{
	return ioctl(UMS_FILENO, IOCTL_EXIT_UMS);
}

static void destroy_worker_key(void *args)
{
	free(args);
	(void) exit_ums_mode();
}

static void create_worker_key(void)
{
	(void) pthread_key_create(&worker_key, destroy_worker_key);
}

static void *worker_wrapper_routine(void *args)
{
	worker_proc_args_t *worker_args = args;

	struct enter_ums_mode_args ums_args = {
		.flags = ENTER_UMS_WORK,
		.ums_complist = worker_args->completion_list
	};

	(void) pthread_once(&worker_key_once, create_worker_key);

	// enter ums mode and deschedule worker thread (suspend here)
	if (enter_ums_mode(&ums_args)) {
		free(args);
		return NULL;
	}

	(void) pthread_setspecific(worker_key, args);

	// worker thread is now active for scheduling
	return worker_args->func(worker_args->args);
}

int ums_pthread_create(pthread_t *thread, ums_attr_t *ums_attr,
			     void *(*func)(void *), void *args)
{
	worker_proc_args_t *ums_args;

	ums_args = malloc(sizeof(*ums_args));
	if (!ums_args)
		return -1;

	ums_args->completion_list = ums_attr->completion_list;
	ums_args->func = func;
	ums_args->args = args;

	return pthread_create(thread,
			      ums_attr->pthread_attr,
			      worker_wrapper_routine,
			      ums_args);
}

int ums_thread_yield(void *scheduler_param)
{
	return ioctl(UMS_FILENO, IOCTL_UMS_YIELD, scheduler_param);
}
