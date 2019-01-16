/* 002.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

static unsigned long max_node;
static unsigned long *node_mask = NULL;
static unsigned long nr_node_mask;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* arg analyze and get max_node */
	max_node = arg_analyze(tc_argc, tc_argv);
	return NULL;
}


RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long l_ret = -1;

	/* max_node check */
	if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	/* create node_mask and tpassert(node_mask malloc) */
	node_mask = (unsigned long *)create_node_mask(max_node, &nr_node_mask);
	tp_assert(node_mask!=NULL, "node_mask malloc error");

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = set_mempolicy(MPOL_DEFAULT, node_mask, 0);
	tp_assert(l_ret==-1, "set_mempolicy not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free nodemask */
	destroy_node_mask(node_mask);
}
