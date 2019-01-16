/* 012.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "test_mck.h"
#include "numa.h"

static unsigned long max_node;
static unsigned long *node_mask = NULL;
static unsigned long nr_node_mask;
static unsigned long *set_node_mask = NULL;

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
	int i;

	/* max_node check */
	if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	/* create node_mask and tpassert(node_mask malloc) */
	node_mask = (unsigned long *)create_node_mask(max_node+1, &nr_node_mask);
	tp_assert(node_mask!=NULL, "node_mask malloc error");

	/* create set_node_mask and tpassert(set_node_mask malloc) */
	set_node_mask = (unsigned long *)create_node_mask(max_node+1, &nr_node_mask);
	tp_assert(set_node_mask!=NULL, "set_node_mask malloc error");

	/* set_node_mask */
	for ( i = 0; i < nr_node_mask; i++ ) {
		/* the last element */
		if ( i == nr_node_mask-1 ) {
			/* 1 bit left shifted value */
			set_node_mask[i] = (1 << (max_node % BITS_PER_LONG + 1)) - 1;
		/* other then the last element */
		} else {
			/* all 1 value */
			set_node_mask[i] = ULLONG_MAX;
		}
	}

	/* print set_node_mask */
	printf("set_node_mask =");
	for ( i = nr_node_mask-1; i >= 0; i-- ) {
		printf(" %lu", set_node_mask[i]);
	}
	printf("\n");

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = set_mempolicy(MPOL_BIND, set_node_mask, max_node+1);
	tp_assert(l_ret==0, "set_mempolicy error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = -1;
	i_ret = get_mempolicy(&mode, node_mask, max_node+1, NULL, 0);
	tp_assert(i_ret==0, "get_mempolicy error");
	tp_assert(mode==MPOL_BIND, "mode != MPOL_BIND");
	printf("node_mask =");
	for ( i = nr_node_mask-1; i >= 0; i-- ) {
		printf(" %lu", node_mask[i]);
	}
	printf("\n");

	/* node_mask check */
	i_ret= -1;
	i_ret= node_mask_check(node_mask, nr_node_mask, max_node);
	tp_assert(i_ret==0, "node_mask error");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free nodemask */
	destroy_node_mask(node_mask);
	destroy_node_mask(set_node_mask);
}
