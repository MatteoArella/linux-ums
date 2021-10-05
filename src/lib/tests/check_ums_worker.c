// SPDX-License-Identifier: AGPL-3.0-only

#include <check.h>

#include <stdlib.h>
#include <ums.h>

ums_completion_list_t comp_list;
pthread_t worker;

void setup(void)
{
	create_ums_completion_list(&comp_list);
}

void teardown(void)
{
	delete_ums_completion_list(&comp_list);
}

static void *work_pthread_proc(void *arg)
{
	return NULL;
}

static void *work_pthread_yield_proc(void *arg)
{
	ck_assert_int_eq(ums_thread_yield(NULL), 0);
}

START_TEST(test_enter_ums_worker_mode)
{
	ums_attr_t ums_attr = { 0 };

	ums_attr.completion_list = comp_list;

	ck_assert_int_eq(
		ums_pthread_create(&worker, &ums_attr, work_pthread_proc, NULL),
		0
	);

	pthread_join(worker, NULL);
}
END_TEST

START_TEST(test_enter_ums_worker_mode_bad)
{
	ums_attr_t ums_attr = { 0 };

	ums_attr.completion_list = comp_list;

	ck_assert_int_eq(
		ums_pthread_create(&worker, NULL, work_pthread_proc, NULL),
		-1
	);
	ck_assert_int_eq(
		ums_pthread_create(&worker, &ums_attr, NULL, NULL),
		-1
	);
}
END_TEST

START_TEST(test_ums_worker_yield)
{
	ums_attr_t ums_attr = { 0 };

	ums_attr.completion_list = comp_list;

	ck_assert_int_eq(
		ums_pthread_create(&worker,
				   &ums_attr,
				   work_pthread_yield_proc,
				   NULL),
		0
	);

	pthread_join(worker, NULL);
}
END_TEST

Suite *create_ums_worker_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("UMS Worker Suite");

	tc_core = tcase_create("UMS Worker");

	tcase_add_checked_fixture(tc_core, setup, teardown);
	tcase_add_test(tc_core, test_enter_ums_worker_mode);
	tcase_add_test(tc_core, test_enter_ums_worker_mode_bad);
	tcase_add_test(tc_core, test_ums_worker_yield);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = create_ums_worker_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
