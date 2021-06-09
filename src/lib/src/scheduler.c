#include "private.h"

static inline int dequeue_ums_sched_event(struct ums_sched_dqevent_args *event)
{
	return ioctl(UMS_FILENO, IOCTL_UMS_SCHED_DQEVENT, event);
}

int enter_ums_scheduling_mode(
	ums_scheduler_startup_info_t *scheduler_startup_info)
{
	struct enter_ums_mode_args enter_args = {
		.flags = ENTER_UMS_SCHED,
		.ums_complist = scheduler_startup_info->completion_list
	};
	struct ums_sched_dqevent_args dqevent_args;
	ums_sched_id_t sched;
	struct ums_sched_event *event;
	int retval;

	if (enter_ums_mode(&enter_args))
		return -1;

	sched = enter_args.ums_scheduler;
	dqevent_args.ums_scheduler = sched;

	for (;;) {
		if (dequeue_ums_sched_event(&dqevent_args)) {
			if (errno == EINTR) continue;
			else return -1;
		}

		event = &dqevent_args.event;

		// proxies event to ums scheduler entry point
		switch (event->type) {
		case SCHEDULER_STARTUP:
			scheduler_startup_info->ums_scheduler_entry_point(
				UMS_SCHEDULER_STARTUP,
				NULL,
				scheduler_startup_info->scheduler_param
			);
			break;
		case THREAD_BLOCKED:
			break;
		case THREAD_YIELD:
			break;
		default:
			break;
		}
	}

	return 0;
}

int execute_ums_thread(ums_context_t ums_context)
{
	errno = ENOSYS;
	return -1;
}
