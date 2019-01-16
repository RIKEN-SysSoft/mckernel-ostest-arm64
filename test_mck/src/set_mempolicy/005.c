/* 005.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

static unsigned long max_node;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* arg analyze and get max_node */
	max_node = arg_analyze(tc_argc, tc_argv);
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long l_ret = -1;
	unsigned long set_node_mask = 0x0C;

	/* max_node check */
	if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	if (max_node < 4) {
		printf("this TP is must have max_node of 4 or more.\n");
		tp_assert(0, "max_node error");
	}

	printf("set_node_mask = %lu\n", set_node_mask);

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = set_mempolicy(MPOL_BIND, &set_node_mask, 2);
	tp_assert(l_ret==-1, "set_mempolicy not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
