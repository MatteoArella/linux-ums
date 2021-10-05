// SPDX-License-Identifier: AGPL-3.0-only

#include <check.h>

#include <stdlib.h>
#include <ums.h>

typedef struct ums_scheduler_startup_info_tcase_t {
	ums_scheduler_startup_info_t *sched_info;
	int expected_retval;
} ums_scheduler_startup_info_tcase_t;

ums_completion_list_t comp_list;
ums_scheduler_startup_info_t sched_info;
ums_scheduler_startup_info_tcase_t sched_info_tcase = {
	.sched_info = &sched_info
};
pthread_t scheduler;

ums_attr_t ums_attr = { 0 };

void setup(void)
{
	create_ums_completion_list(&comp_list);
	sched_info.completion_list = comp_list;
	ums_attr.completion_list = comp_list;
}

void teardown(void)
{
	delete_ums_completion_list(&comp_list);
}

static void *sched_pthread_proc(void *arg)
{
	ums_scheduler_startup_info_tcase_t *sched_info = arg;

	ck_assert_int_eq(enter_ums_scheduling_mode(sched_info->sched_info),
			 sched_info->expected_retval);

	return NULL;
}

static void startup_sched_entry_proc(ums_reason_t reason,
				     ums_activation_t *activation,
				     void *args)
{
	pthread_exit(NULL);
}

static void dequeue_comp_list_sched_entry_proc(ums_reason_t reason,
					       ums_activation_t *activation,
					       void *args)
{
	ums_context_t context;

	ck_assert_int_eq(
		dequeue_ums_completion_list_items(comp_list, &context),
		0
	);
	pthread_exit(NULL);
}

static void dequeue_comp_list_sched_entry_proc_bad(ums_reason_t reason,
					       ums_activation_t *activation,
					       void *args)
{
	ck_assert_int_eq(
		dequeue_ums_completion_list_items(comp_list, NULL),
		-1
	);
	pthread_exit(NULL);
}

static void next_comp_list_sched_entry_proc(ums_reason_t reason,
					    ums_activation_t *activation,
					    void *args)
{
	ums_context_t context_list;

	ck_assert_int_eq(
		dequeue_ums_completion_list_items(comp_list, &context_list),
		0
	);
	ck_assert_int_gt(
		get_next_ums_list_item(context_list),
		0
	);
	pthread_exit(NULL);
}

static void *work_pthread_proc(void *arg)
{
	return NULL;
}

START_TEST(test_enter_ums_scheduling_mode)
{
	sched_info.ums_scheduler_entry_point = startup_sched_entry_proc;
	sched_info_tcase.expected_retval = 0;

	pthread_create(&scheduler, NULL, sched_pthread_proc,
		       &sched_info_tcase);

	pthread_join(scheduler, NULL);
}
END_TEST

START_TEST(test_enter_ums_scheduling_mode_bad)
{
	ums_scheduler_startup_info_t *old = sched_info_tcase.sched_info;
	sched_info_tcase.sched_info = NULL;
	sched_info_tcase.expected_retval = -1;

	pthread_create(&scheduler, NULL, sched_pthread_proc,
		       &sched_info_tcase);

	pthread_join(scheduler, NULL);

	sched_info_tcase.sched_info = old;
}
END_TEST

START_TEST(test_ums_scheduler_dequeue_comp_list)
{
	pthread_t worker;

	sched_info.ums_scheduler_entry_point =
					dequeue_comp_list_sched_entry_proc;
	sched_info_tcase.expected_retval = 0;

	ck_assert_int_eq(
		ums_pthread_create(&worker, &ums_attr, work_pthread_proc, NULL),
		0
	);

	pthread_create(&scheduler, NULL, sched_pthread_proc,
		       &sched_info_tcase);

	pthread_join(worker, NULL);
	pthread_join(scheduler, NULL);
}
END_TEST

START_TEST(test_ums_scheduler_dequeue_comp_list_bad)
{
	sched_info.ums_scheduler_entry_point =
					dequeue_comp_list_sched_entry_proc_bad;
	sched_info_tcase.expected_retval = 0;

	pthread_create(&scheduler, NULL, sched_pthread_proc,
		       &sched_info_tcase);

	pthread_join(scheduler, NULL);
}
END_TEST

START_TEST(test_ums_scheduler_next_comp_list)
{
	pthread_t worker1, worker2;

	sched_info.ums_scheduler_entry_point =
					next_comp_list_sched_entry_proc;
	sched_info_tcase.expected_retval = 0;

	ck_assert_int_eq(
		ums_pthread_create(&worker1, &ums_attr,
				   work_pthread_proc, NULL),
		0
	);
	ck_assert_int_eq(
		ums_pthread_create(&worker2, &ums_attr,
				   work_pthread_proc, NULL),
		0
	);

	pthread_create(&scheduler, NULL, sched_pthread_proc,
		       &sched_info_tcase);

	pthread_join(worker1, NULL);
	pthread_join(worker2, NULL);
	pthread_join(scheduler, NULL);
}
END_TEST

Suite *create_ums_scheduler_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("UMS Scheduler Suite");

	tc_core = tcase_create("UMS Scheduler");

	tcase_add_checked_fixture(tc_core, setup, teardown);
	tcase_add_test(tc_core, test_enter_ums_scheduling_mode);
	tcase_add_test(tc_core, test_enter_ums_scheduling_mode_bad);
	tcase_add_test(tc_core, test_ums_scheduler_dequeue_comp_list);
	tcase_add_test(tc_core, test_ums_scheduler_dequeue_comp_list_bad);
	tcase_add_test(tc_core, test_ums_scheduler_next_comp_list);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = create_ums_scheduler_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
