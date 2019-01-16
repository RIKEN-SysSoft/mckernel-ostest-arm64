/* 000.c COPYRIGHT FUJITSU LIMITED 2017 */
#include "test_mck.h"
#include "testsuite.h"

static int dirfd = -1;
static const char *pathname = "";
static struct stat buf;
static int flags = 0;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{	
	int ret;
	
	dirfd = open("/proc", O_DIRECTORY);
	tp_assert(dirfd != -1, "open error");
	
	ret = fstatat(dirfd, pathname, &buf, flags);
	tp_assert(ret == -1, "unexpected fstatat success");	
	
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (dirfd != -1) {
		close(dirfd);
	}
}	
