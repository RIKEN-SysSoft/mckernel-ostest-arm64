/* 017.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	int mode = -1;
	int i_ret = -1;
	int i;
	char buf[PAGE_SIZE] = {0};

	/* max_node check */
	if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = get_mempolicy(&mode, NULL, 0, buf, MPOL_F_ADDR | MPOL_F_NODE);
	tp_assert(i_ret==0, "get_mempolicy error");

	printf("node ID = %d\n", mode);

	/* node ID check */
	i_ret = -1;
	for (i = 0; i < max_node; i++) {
		if ( mode == i ) {
			i_ret = 0;
			break;
		}
	}
		
	tp_assert(i_ret==0, "node ID error");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
