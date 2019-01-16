/* 009.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	int mode = -1;
	int i_ret = -1;
	long l_ret = -1;
	unsigned long set_node_mask = 0x03;

	/* max_node check */
	if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	if (max_node < 2) {
		printf("this TP is must have max_node of 2 or more.\n");
		tp_assert(0, "max_node error");
	}

	printf("set_node_mask = %lu\n", set_node_mask);

	/* create node_mask and tpassert(node_mask malloc) */
	node_mask = (unsigned long *)create_node_mask(max_node, &nr_node_mask);
	tp_assert(node_mask!=NULL, "node_mask malloc error");

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = set_mempolicy(MPOL_BIND | MPOL_F_STATIC_NODES, &set_node_mask, BITS_PER_LONG);
	tp_assert(l_ret==0, "set_mempolicy error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = -1;
	i_ret = get_mempolicy(&mode, node_mask, max_node, NULL, 0);
	tp_assert(i_ret==0, "get_mempolicy error");
	tp_assert(mode==(MPOL_BIND | MPOL_F_STATIC_NODES), "mode != MPOL_BIND");
	tp_assert(node_mask[0]==set_node_mask, "node_mask != set_node_mask");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free nodemask */
	destroy_node_mask(node_mask);
}
