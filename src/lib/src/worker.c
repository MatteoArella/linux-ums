#include "private.h"

int ums_pthread_create(pthread_t *thread, ums_attr_t *ums_attr,
			     void *(*func)(void *), void *args)
{
	errno = ENOSYS;
	return -1;
}

int ums_thread_yield(void *scheduler_param)
{
	errno = ENOSYS;
	return -1;
}
