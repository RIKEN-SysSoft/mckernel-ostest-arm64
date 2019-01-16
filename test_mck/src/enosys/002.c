/* 002.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "test_mck.h"
#include "testsuite.h"

static int sysnums[] = {
	__NR_migrate_pages,
};

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int num;
	int stat;
	int i;

	for (i = 0; i < sizeof(sysnums)/sizeof(sysnums[0]); i++) {
		stat = syscall(sysnums[i]);
		tp_assert(stat == -1, "syscall error");
		num = errno;
		tp_assert(num == ENOSYS, "errno error");
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
