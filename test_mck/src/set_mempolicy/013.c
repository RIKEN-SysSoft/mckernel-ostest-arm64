/* 013.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

static unsigned long nr_node_mask;
static unsigned long *set_node_mask = NULL;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long l_ret = -1;
	unsigned long sys_max_node = PAGE_SIZE * BITS_PER_BYTE;

	/* create set_node_mask and tpassert(set_node_mask malloc) */
	set_node_mask = (unsigned long *)create_node_mask(sys_max_node+1, &nr_node_mask);
	tp_assert(set_node_mask!=NULL, "set_node_mask malloc error");
	set_node_mask[0] = 0x01;

	printf("sys_max_node = %lu\n", sys_max_node);

	/* set_mempolicy and tp_assert(set_mempolicy) */
	l_ret = set_mempolicy(MPOL_BIND, set_node_mask, sys_max_node+2);
	tp_assert(l_ret==-1, "set_mempolicy not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free node_mask */
	destroy_node_mask(set_node_mask);
}
