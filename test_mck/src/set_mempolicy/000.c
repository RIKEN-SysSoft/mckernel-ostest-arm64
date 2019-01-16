/* 000.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	int mode = -1, i_ret = -1, i;
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

	/* create node_mask and tpassert(node_mask malloc) */
	node_mask = (unsigned long *)create_node_mask(max_node, &nr_node_mask);
	tp_assert(node_mask!=NULL, "node_mask malloc error");

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = -1;
	l_ret = set_mempolicy(MPOL_BIND, &set_node_mask, BITS_PER_LONG);
	tp_assert(l_ret==0, "set_mempolicy(MPOL_BIND) error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = -1;
	i_ret = get_mempolicy(&mode, node_mask, max_node, NULL, 0);
	tp_assert(i_ret==0, "get_mempolicy1 error");
	tp_assert(mode==MPOL_BIND, "mode != MPOL_BIND");
	tp_assert(node_mask[0]==set_node_mask, "node_mask != set_node_mask");

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = -1;
	l_ret = set_mempolicy(MPOL_DEFAULT, NULL, 0);
	tp_assert(l_ret==0, "set_mempolicy(MPOL_DEFAULT) error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	mode = -1;
	i_ret = -1;
	node_mask[0] = 0x01;
	i_ret = get_mempolicy(&mode, node_mask, max_node, NULL, 0);
	tp_assert(i_ret==0, "get_mempolicy2 error");

	printf("mode = %d\n", mode);

	/* tp_assert(mode) */
	tp_assert(mode==MPOL_DEFAULT, "mode != MPOL_DEFAULT");

	/* print nodemask */
	printf("nodemask =");
	for ( i = nr_node_mask-1; i >= 0; i-- ) {
		printf(" %lu", node_mask[i]);

		/* tp_assert(node_mask) */
		tp_assert(node_mask[i]==0, "\nnode_mask != 0");
	}
	printf("\n");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free node_mask */
	destroy_node_mask(node_mask);
}
