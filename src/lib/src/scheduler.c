#include "private.h"

static inline int dequeue_ums_sched_event(struct ums_sched_event *event)
{
	return ioctl(UMS_FILENO, IOCTL_SCHED_DQEVENT, event);
}

int enter_ums_scheduling_mode(
	ums_scheduler_startup_info_t *scheduler_startup_info)
{
	struct enter_ums_mode_args args = {
		.flags = ENTER_UMS_SCHED,
		.ums_complist = scheduler_startup_info->completion_list
	};
	int retval;
	struct ums_sched_event event;

	if (enter_ums_mode(&args))
		return -1;

	for (;;) {
		// dequeue ums scheduler event
		if (dequeue_ums_sched_event(&event)) {
			if (errno == EINTR) continue;
			else return -1;
		}

		// proxies event to ums scheduler entry point
		switch (event.type) {
		case SCHEDULER_STARTUP:
			scheduler_startup_info->ums_scheduler_entry_point(
				UMS_SCHEDULER_STARTUP,
				NULL,
				scheduler_startup_info->scheduler_param
			);
			break;
		case THREAD_BLOCKED:
		case THREAD_YIELD:
			break;
		default:
			break;
		}
	}

	// never reach here. Needed or compiler will complain about it
	return 0;
}

int execute_ums_thread(ums_context_t ums_context)
{
	errno = ENOSYS;
	return -1;
}
