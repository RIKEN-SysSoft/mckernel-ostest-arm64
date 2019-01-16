/* 003.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "test_mck.h"
#include "numa.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	unsigned long len = ULLONG_MAX & ~(PAGE_SIZE - 1);
	long l_ret = -1;
	char tmp_buf[PAGE_SIZE * 2] = { 0 };
	char *buf = NULL;
	buf = (char *)(((unsigned long)tmp_buf + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

	/* mbind and tpassert(mbind) */
	l_ret = mbind(buf, len, MPOL_DEFAULT, NULL, 0, 0);
	tp_assert(l_ret==-1, "mbind not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
