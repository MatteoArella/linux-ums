#ifndef UMS_IOCTL_H
#define UMS_IOCTL_H

#include <linux/ioctl.h>

typedef int ums_context_id_t;
typedef int ums_comp_list_id_t;

#define ENTER_UMS_SCHED		0x1
#define ENTER_UMS_WORK		0x2

typedef enum ums_sched_event_e {
	SCHEDULER_STARTUP = 0,
	THREAD_BLOCKED,
	THREAD_YIELD
} ums_sched_event_t;

struct enter_ums_mode_args {
	int flags;
	ums_comp_list_id_t ums_complist;
	ums_context_id_t ums_context;
};

struct ums_sched_event {
	ums_sched_event_t type;
};

#define UMS_DEV_NAME		"ums"

#define UMS_IOC_MAGIC		0xF0

#define IOCTL_CREATE_UMS_CTX	_IO(UMS_IOC_MAGIC, 1)
#define IOCTL_CREATE_UMS_CLIST	_IO(UMS_IOC_MAGIC, 2)
#define IOCTL_ENTER_UMS		_IOR(UMS_IOC_MAGIC, 3, \
				     struct enter_ums_mode_args *)

#define IOCTL_SCHED_DQEVENT	_IOW(UMS_IOC_MAGIC, 4, \
				     struct ums_sched_event *)

#define IOCTL_DEQUEUE_UMS_CLIST	_IO(UMS_IOC_MAGIC, 5)
#define IOCTL_NEXT_UMS_CLIST	_IO(UMS_IOC_MAGIC, 6)
#define IOCTL_EXEC_UMS_CTX	_IO(UMS_IOC_MAGIC, 7)
#define IOCTL_UMS_YIELD		_IO(UMS_IOC_MAGIC, 8)
#define IOCTL_DELETE_UMS_CLIST	_IO(UMS_IOC_MAGIC, 9)
#define IOCTL_DELETE_UMS_CTX	_IO(UMS_IOC_MAGIC, 10)

#endif /* UMS_IOCTL_H */
