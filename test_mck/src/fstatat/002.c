/* 002.c COPYRIGHT FUJITSU LIMITED 2017 */
#include "test_mck.h"
#include "testsuite.h"

static int dirfd = AT_FDCWD;
static const char *pathname = "";
static struct stat buf;
static int flags = 0;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{	
	int ret;
	
	ret = fstatat(dirfd, pathname, &buf, flags);
	tp_assert(ret == -1, "unexpected fstatat success");	
	
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

