/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_DEVICE_H
#define UMS_DEVICE_H

#include "uapi/ums/ums_ioctl.h"

/**
 * @brief Register UMS miscellaneous device
 *
 * @return 0 in case of success, -ERRNO otherwise.
 */
int register_ums_device(void);

/**
 * @brief Unregister UMS miscellaneous device
 *
 * @return 0 in case of success, -ERRNO otherwise.
 */
void unregister_ums_device(void);

#endif /* UMS_DEVICE_H */
