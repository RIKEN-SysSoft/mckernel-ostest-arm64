/* 016.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	int mode = -1;
	int i_ret = -1;
	char buf[PAGE_SIZE] = {0};

	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = get_mempolicy(&mode, NULL, 0, buf, 0);
	tp_assert(i_ret==-1, "get_mempolicy not failed");
	tp_assert(errno==EINVAL, "get_mempolicy error");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
