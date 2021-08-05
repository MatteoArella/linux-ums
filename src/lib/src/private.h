/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_PRIVATE_H
#define UMS_PRIVATE_H

#include "../include/ums.h"

#include "ums/ums_ioctl.h"

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

/**
 * UMS device file descriptor
 */
extern int UMS_FILENO;

static __always_inline int enter_ums_mode(struct enter_ums_mode_args *args)
{
	return ioctl(UMS_FILENO, IOCTL_ENTER_UMS, args);
}

#endif /* UMS_PRIVATE_H */
