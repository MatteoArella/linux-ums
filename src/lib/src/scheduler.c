#include "private.h"

static inline int dequeue_ums_sched_event(struct ums_sched_event *event)
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
	struct ums_sched_event event;
	ums_activation_t scheduler_activation;

	if (enter_ums_mode(&enter_args))
		return -1;

	for (;;) {
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
			break;
		case THREAD_YIELD:
			scheduler_activation.context =
						event.yield_params.context;

			scheduler_startup_info->ums_scheduler_entry_point(
				UMS_SCHEDULER_THREAD_YIELD,
				&scheduler_activation,
				event.yield_params.scheduler_params
			);
			break;
		case THREAD_TERMINATED:
			scheduler_activation.context =
						event.end_params.context;

			scheduler_startup_info->ums_scheduler_entry_point(
				UMS_SCHEDULER_THREAD_END,
				&scheduler_activation,
				NULL
			);
			break;
		default:
			break;
		}
	}

	return 0;
}

int execute_ums_thread(ums_context_t ums_context)
{
	return ioctl(UMS_FILENO, IOCTL_EXEC_UMS_CTX, ums_context);
}
