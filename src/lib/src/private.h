#ifndef UMS_PRIVATE_H
#define UMS_PRIVATE_H

#include "../include/ums.h"

#include "ums_ioctl.h"

#include <stddef.h>
#include <errno.h>
#include <sys/ioctl.h>

extern int UMS_FILENO;

inline int enter_ums_mode(struct enter_ums_mode_args *args)
{
	return ioctl(UMS_FILENO, IOCTL_ENTER_UMS, args);
}

#endif /* UMS_PRIVATE_H */