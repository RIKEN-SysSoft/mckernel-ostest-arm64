/* 008.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	l_ret = set_mempolicy(MPOL_BIND, NULL, 0);
	tp_assert(l_ret==-1, "set_mempolicy not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
