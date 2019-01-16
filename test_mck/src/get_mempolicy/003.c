/* 003.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>

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
	int i;

	/* max_node check */
	if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	printf("maxnode = %lu\n", max_node);
	
	/* create node_mask and tpassert(node_mask malloc) */
	node_mask = (unsigned long *)create_node_mask(max_node, &nr_node_mask);
	tp_assert(node_mask!=NULL, "node_mask malloc error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	node_mask[0] = 0x01;
	i_ret = get_mempolicy(&mode, node_mask, max_node, NULL, 0);
	tp_assert(i_ret==0, "get_mempolicy error");

	/* tp_assart(mode) */
	tp_assert(mode==MPOL_DEFAULT, "mode != MPOL_DEFAULT");

	/* print nodemask */
	printf("nodemask =");
	for ( i = nr_node_mask-1; i >= 0; i-- ) {
		printf(" %lu", node_mask[i]);
		
		/* tp_assert(nodemask) */
		tp_assert(node_mask[i]==0, "\nnode_mask != 0");
	}
	printf("\n");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free nodemask */
	destroy_node_mask(node_mask);
}
