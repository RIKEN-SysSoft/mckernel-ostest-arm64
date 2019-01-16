/* 017.c COPYRIGHT FUJITSU LIMITED 2017 */
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

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = set_mempolicy(~(MPOL_DEFAULT | MPOL_BIND | MPOL_INTERLEAVE | MPOL_PREFERRED), NULL, 0);
	tp_assert(l_ret==-1, "set_mempolicy not failed");
	tp_assert(errno==EINVAL, "errno not EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
