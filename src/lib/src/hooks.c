// SPDX-License-Identifier: AGPL-3.0-only

/**
 * @file hooks.c
 */

#include "private.h"

#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

int UMS_FILENO = -1;

/**
 * @brief Open UMS file descriptor
 */
static void ums_fileno_open(void)
{
	UMS_FILENO = open("/dev/" UMS_DEV_NAME, O_RDONLY);
}

/**
 * @brief Close UMS file descriptor
 */
static void ums_fileno_close(void)
{
	close(UMS_FILENO);
}

/**
 * @brief Handler executed in the child process after fork processing completes
 */
static void ums_atfork_child_handler(void)
{
	ums_fileno_close();
	ums_fileno_open();
}

/**
 * @brief UMS library ctor
 * 
 * This function initializes UMS library.
 */
__attribute__((constructor))
static void ums_init(void)
{
	ums_fileno_open();
	(void) pthread_atfork(NULL, NULL, ums_atfork_child_handler);
}

/**
 * @brief UMS library dtor
 * 
 * This function deinitializes UMS library.
 */
__attribute__((destructor))
static void ums_exit(void)
{
	ums_fileno_close();
}
