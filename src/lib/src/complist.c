#include "private.h"

int create_ums_completion_list(ums_completion_list_t *completion_list)
{
	return 0;
}

int dequeue_ums_completion_list_items(ums_completion_list_t completion_list,
				      ums_context_t ums_thread_list)
{
	errno = ENOSYS;
	return -1;
}

int get_next_ums_list_item(ums_context_t prev, ums_context_t *next)
{
	errno = ENOSYS;
	return -1;
}

int delete_ums_completion_list(ums_completion_list_t *completion_list)
{
	errno = ENOSYS;
	return -1;
}
