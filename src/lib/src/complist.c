// SPDX-License-Identifier: AGPL-3.0-only

#include "private.h"

int create_ums_completion_list(ums_completion_list_t *completion_list)
{
	if (!completion_list) {
		errno = EFAULT;
		return -1;
	}

	return ioctl(UMS_FILENO, IOCTL_CREATE_UMS_CLIST, completion_list);
}

int dequeue_ums_completion_list_items(ums_completion_list_t completion_list,
				      ums_context_t *ums_thread_list)
{
	struct dequeue_ums_complist_args dequeue_args = {
		.ums_complist = completion_list
	};
	int retval;

	if (!ums_thread_list) {
		errno = EFAULT;
		return -1;
	}

	retval = ioctl(UMS_FILENO, IOCTL_DEQUEUE_UMS_CLIST, &dequeue_args);
	if (!retval)
		*ums_thread_list = dequeue_args.ums_context;

	return retval;
}

ums_context_t get_next_ums_list_item(ums_context_t context)
{
	struct ums_next_context_list_args next_context_args = {
		.ums_context = context
	};
	int retval;

	retval = ioctl(UMS_FILENO,
		       IOCTL_NEXT_UMS_CTX_LIST,
		       &next_context_args);
	if (retval)
		return retval;

	return next_context_args.ums_next_context == -1 ? 0 :
			next_context_args.ums_next_context;
}

int delete_ums_completion_list(ums_completion_list_t *completion_list)
{
	int retval;

	if (!completion_list) {
		errno = EFAULT;
		return -1;
	}

	retval = ioctl(UMS_FILENO,
		       IOCTL_DELETE_UMS_CLIST,
		       *completion_list);

	if (!retval)
		*completion_list = 0;

	return retval;
}
