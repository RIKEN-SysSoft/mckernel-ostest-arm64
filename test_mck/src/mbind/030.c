/* 030.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long l_ret = -1;
	char tmp_buf[PAGE_SIZE * 2] = { 0 };
	char *buf = NULL;
	buf = (char *)(((unsigned long)tmp_buf + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

	/* mbind and tpassert(mbind) */
	l_ret = mbind(buf, PAGE_SIZE,
		~(MPOL_DEFAULT | MPOL_BIND | MPOL_INTERLEAVE | MPOL_PREFERRED), NULL, 0, 0);
	tp_assert(l_ret==-1, "mbind not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
