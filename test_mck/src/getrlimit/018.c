/* 018.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

#define DELEGATE_NUMBER 0

SETUP_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	getrlimit_tc tc = {
		RLIMIT_NLIMITS,
		"RLIMIT_NLIMITS",
		TEST_GETRLIMIT_FAILUER,
		EINVAL,
		"EINVAL"
	};

	int result = 0;
	struct rlimit rlim;

	tp_assert(0 <= tc_num, "invalid tp number.");

	result = getrlimit(tc.resource, &rlim);
	printf("getrlimit(%s)=%d\n", tc.res_str, result);
	if(result != 0){
		printf("errno=%d (McK may return %s(=%d))\n", errno, tc.errno_str, tc.expect_errno);
	}
	tp_assert(result == tc.expect_result, "getrlimit failed. (retval different)");
	tp_assert(errno == tc.expect_errno, "getrlimit failed. (errno different)");

	return NULL;
}

TEARDOWN_ALIAS(TEST_SUITE, TEST_NUMBER, DELEGATE_NUMBER)
