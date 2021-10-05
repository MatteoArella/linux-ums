// SPDX-License-Identifier: AGPL-3.0-only

#include <check.h>

#include <stdlib.h>
#include <ums.h>

START_TEST(test_ums_completion_list_create)
{
	ums_completion_list_t comp_list;

	ck_assert_int_eq(create_ums_completion_list(&comp_list), 0);
	ck_assert_int_eq(delete_ums_completion_list(&comp_list), 0);
	ck_assert_int_eq(delete_ums_completion_list(&comp_list), -1);
}
END_TEST

START_TEST(test_ums_completion_list_create_bad)
{
	ck_assert_int_eq(create_ums_completion_list(NULL), -1);
}
END_TEST

START_TEST(test_ums_completion_list_delete_bad)
{
	ums_completion_list_t comp_list = -1;

	ck_assert_int_eq(delete_ums_completion_list(NULL), -1);
	ck_assert_int_eq(delete_ums_completion_list(&comp_list), -1);
}
END_TEST

Suite *create_ums_completion_list_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("UMS Completion List Suite");

	tc_core = tcase_create("UMS Completion List");

	tcase_add_test(tc_core, test_ums_completion_list_create);
	tcase_add_test(tc_core, test_ums_completion_list_create_bad);
	tcase_add_test(tc_core, test_ums_completion_list_delete_bad);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = create_ums_completion_list_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
