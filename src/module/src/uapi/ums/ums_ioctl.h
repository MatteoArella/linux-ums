#ifndef UMS_IOCTL_H
#define UMS_IOCTL_H

#include <linux/ioctl.h>
#include <linux/types.h>

/**
 * UMS completion list id.
 */
typedef int ums_comp_list_id_t;

/**
 * Enter UMS mode as UMS scheduler thread
 */
#define ENTER_UMS_SCHED		0x1

/**
 * Enter UMS mode as UMS worker thread
 */
#define ENTER_UMS_WORK		0x2

/**
 * struct for enter UMS mode
 */
struct enter_ums_mode_args {
	/**
	 * specify caller UMS mode
	 */
	int flags;

	/**
	 * the completion list to be associated with the caller
	 */
	ums_comp_list_id_t ums_complist;
};

/**
 * UMS scheduler event type
 */
typedef enum ums_sched_event_type_e {
	/** UMS scheduler startup event */
	SCHEDULER_STARTUP = 0,
	/** UMS worker yielded event */
	THREAD_YIELD,
	/** UMS worker terminated event */
	THREAD_TERMINATED
} ums_sched_event_type_t;

/**
 * UMS scheduler event associated to a UMS worker thread that yielded
 */
struct ums_thread_yield_args {
	/**
	 * UMS context of the worker thread that yielded.
	 */
	pid_t context;

	/**
	 * parameter passed from the UMS worker thread.
	 */
	void *scheduler_params;
};

/**
 * UMS scheduler event associated to a UMS worker thread that terminated
 */
struct ums_thread_end_args {
	/**
	 * UMS context of the worker thread that terminated.
	 */
	pid_t context;
};

/**
 * UMS scheduler event
 */
struct ums_sched_event {
	ums_sched_event_type_t type;
	/**
	 * UMS scheduler event parameters
	 */
	union {
		struct ums_thread_yield_args yield_params;
		struct ums_thread_end_args end_params;
	};
};

/**
 * Dequeue UMS completion list args
 */
struct dequeue_ums_complist_args {
	/**
	 * the completion list from which dequeuying UMS contexts
	 */
	ums_comp_list_id_t ums_complist;

	/**
	 * dequeued UMS context
	 */
	pid_t ums_context;
};

/**
 * Get next UMS context list args
 */
struct ums_next_context_list_args {
	/**
	 * current UMS context in the dequeued UMS thread list
	 */
	pid_t ums_context;

	/**
	 * next UMS context in the dequeued UMS thread list
	 */
	pid_t ums_next_context;
};

/**
 * UMS device name
 */
#define UMS_DEV_NAME		"ums"

#define UMS_IOC_MAGIC		0xF0

/**
 * Create UMS completion list IOCTL number
 */
#define IOCTL_CREATE_UMS_CLIST	_IOR(UMS_IOC_MAGIC, 1, \
				     ums_comp_list_id_t *)

/**
 * Enter UMS mode IOCTL number
 */
#define IOCTL_ENTER_UMS		_IOW(UMS_IOC_MAGIC, 2, \
				     struct enter_ums_mode_args *)

/**
 * Dequeue UMS scheduler event IOCTL number
 */
#define IOCTL_UMS_SCHED_DQEVENT	_IOR(UMS_IOC_MAGIC, 3, \
				      struct ums_sched_event *)

/**
 * Dequeue UMS context from completion list IOCTL number
 */
#define IOCTL_DEQUEUE_UMS_CLIST	_IOWR(UMS_IOC_MAGIC, 4, \
				      struct dequeue_ums_complist_args *)

/**
 * Get next completion list UMS context IOCTL number
 */
#define IOCTL_NEXT_UMS_CTX_LIST	_IOWR(UMS_IOC_MAGIC, 5, \
				    struct ums_next_context_list_args *)

/**
 * Execute UMS worker thread IOCTL number
 */
#define IOCTL_EXEC_UMS_CTX	_IOW(UMS_IOC_MAGIC, 6, pid_t)

/**
 * UMS worker thread yield IOCTL number
 */
#define IOCTL_UMS_YIELD		_IOW(UMS_IOC_MAGIC, 7, void *)

/**
 * Worker thread exit UMS mode IOCTL number
 */
#define IOCTL_EXIT_UMS		_IO(UMS_IOC_MAGIC, 8)

/**
 * Delete UMS completion list IOCTL number
 */
#define IOCTL_DELETE_UMS_CLIST	_IOW(UMS_IOC_MAGIC, 9, \
				     ums_comp_list_id_t)

#endif /* UMS_IOCTL_H */
