// SPDX-License-Identifier: AGPL-3.0-only

/**
 * @file hooks.c
 */

#include "private.h"

#include <unistd.h>
#include <fcntl.h>

int UMS_FILENO = -1;

/**
 * @brief UMS library ctor
 * 
 * This function initializes UMS library opening the UMS device.
 */
__attribute__((constructor))
static void ums_init(void)
{
	UMS_FILENO = open("/dev/" UMS_DEV_NAME, O_RDONLY);
}

/**
 * @brief UMS library dtor
 * 
 * This function deinitializes UMS library closing the UMS device.
 */
__attribute__((destructor))
static void ums_exit(void)
{
	close(UMS_FILENO);
}
