/* 014.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

static unsigned long nr_node_mask;
static unsigned long *mbind_node_mask = NULL;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	long l_ret = -1;
	char tmp_buf[PAGE_SIZE * 2] = { 0 };
	char *buf = NULL;
	unsigned long sys_max_node = PAGE_SIZE * BITS_PER_BYTE;
	buf = (char *)(((unsigned long)tmp_buf + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

	/* create mbind_node_mask and tpassert(mbind_node_mask malloc) */
	mbind_node_mask = (unsigned long *)create_node_mask(sys_max_node+1, &nr_node_mask);
	tp_assert(mbind_node_mask!=NULL, "mbind_node_mask malloc error");
	mbind_node_mask[0] = 0x01;

	/* mbind_mempolicy and tp_assert(mbind) */
	l_ret = mbind(buf, PAGE_SIZE, MPOL_BIND, mbind_node_mask, sys_max_node+2, 0);
	tp_assert(l_ret==-1, "mbind not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free node_mask */
	destroy_node_mask(mbind_node_mask);
}
