/* 009.c COPYRIGHT FUJITSU LIMITED 2018 */
/* util_migrate_inter_kernel use UTI_FLAG_EXCLUSIVE_CPU flag */
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	uti_attr_t attr;

	memset(&attr, 0, sizeof(attr));

	attr.flags |= UTI_FLAG_EXCLUSIVE_CPU;
	ret = util_migrate_inter_kernel(&attr);
	if (ret != 0) {
		perror("util_migrate_inter_kernel:");
		tp_assert(0, "util_migrate_inter_kernel error.");
	}

	ret = get_system();
	if (ret == 0) {
		printf("this process running on McKernel.\n");
		tp_assert(0, "migrate failed.");
	} else if (ret == -1) {
		printf("this process running on HOST-Linux.\n");
	} else {
		printf("get_system illegal return value(%d)\n", ret);
		tp_assert(0, "get_system error.");
	}
	return NULL;
}
