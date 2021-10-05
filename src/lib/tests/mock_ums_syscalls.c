// SPDX-License-Identifier: AGPL-3.0-only

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <errno.h>

#include "ums/ums_ioctl.h"

// TODO: implement better test fixtures
static int UMS_FILENO = (INT_MAX >> 2);
static ums_comp_list_id_t ums_complist_id = 1;
static pid_t ums_context = 1;
static pid_t next_ums_context = 1;

int __real_open(const char *pathname, int flags, ...);
int __real_close(int fd);
int __real_ioctl(int fd, unsigned long request, ...);

int __wrap_open(const char *pathname, int flags, ...)
{
	va_list args;
	int retval;

	if (!strncmp(pathname, "/dev/ums", 8))
		return UMS_FILENO;

	va_start(args, flags);
	retval = __real_open(pathname, flags, args);
	va_end(args);

	return retval;
}

int __wrap_close(int fd)
{
	if (fd == UMS_FILENO)
		return 0;

	return __real_close(fd);
}

static int ums_ioctl(int fd, unsigned long request, unsigned long arg)
{
	ums_comp_list_id_t comp_list;
	ums_comp_list_id_t *comp_listp;
	struct enter_ums_mode_args *enter_ums_args;
	struct ums_sched_event *sched_event_args;
	struct dequeue_ums_complist_args *dequeue_args;
	struct ums_next_context_list_args *next_context_args;

	switch (request) {
	case IOCTL_CREATE_UMS_CLIST:
		comp_listp = (ums_comp_list_id_t *) arg;
		*comp_listp = ums_complist_id;
		return 0;
	case IOCTL_DELETE_UMS_CLIST:
		comp_list = (ums_comp_list_id_t) arg;
		/* completion list not found */
		if (comp_list != ums_complist_id) {
			errno = EINVAL;
			return -1;
		}
		return 0;
	case IOCTL_ENTER_UMS:
		enter_ums_args = (struct enter_ums_mode_args *) arg;
		if (enter_ums_args->flags == ENTER_UMS_SCHED)
			return 0;
		else if (enter_ums_args->flags == ENTER_UMS_WORK)
			return 0;
		break;
	case IOCTL_UMS_SCHED_DQEVENT:
		return 0;
	case IOCTL_DEQUEUE_UMS_CLIST:
		dequeue_args = (struct dequeue_ums_complist_args *) arg;
		/* completion list not found */
		if (dequeue_args->ums_complist != ums_complist_id) {
			errno = EINVAL;
			return -1;
		}
		dequeue_args->ums_context = ums_context;
		return 0;
	case IOCTL_NEXT_UMS_CTX_LIST:
		next_context_args = (struct ums_next_context_list_args *) arg;

		if (next_context_args->ums_context != ums_context)
			next_context_args->ums_next_context = -1;

		next_context_args->ums_next_context = next_ums_context;

		return 0;
	case IOCTL_UMS_YIELD:
		return 0;
	default:
		errno = ENOTSUP;
		return -1;
	}

	return -1;
}

int __wrap_ioctl(int fd, unsigned long request, ...)
{
	va_list args;
	int retval;

	va_start(args, request);

	if (fd == UMS_FILENO)
		retval = ums_ioctl(fd, request, va_arg(args, unsigned long));
	else
		retval = __real_ioctl(fd, request, args);

	va_end(args);

	return retval;
}
