/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef UMS_TESTS_H
#define UMS_TESTS_H

#include <config.h>

#include <check.h>
#include <ums.h>

Suite *create_ums_completion_list_suite(void);
Suite *create_ums_scheduler_suite(void);
Suite *create_ums_worker_suite(void);

#endif /* UMS_TESTS_H */
