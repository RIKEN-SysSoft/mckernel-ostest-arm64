/* 008.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>

#include "test_mck.h"
#include "numa.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i_ret = -1;
	
	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = get_mempolicy(NULL, NULL, 0, NULL, MPOL_F_MEMS_ALLOWED);
	tp_assert(i_ret==0, "get_mempolicy error");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
