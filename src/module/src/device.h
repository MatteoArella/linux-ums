/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_DEVICE_H
#define UMS_DEVICE_H

#include "uapi/ums_ioctl.h"

int register_ums_device(void);

void unregister_ums_device(void);

#endif /* UMS_DEVICE_H */