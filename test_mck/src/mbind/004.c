/* 004.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	unsigned long mbind_node_mask = 0x03;
	char tmp_buf[PAGE_SIZE * 2] = { 0 };
	char *buf = NULL;
	buf = (char *)(((unsigned long)tmp_buf + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

	/* max_node check */
		if (max_node == 0) {
		usage_print();
		tp_assert(0, "max_node = 0");
	}

	if (max_node < 2) {
		printf("this TP is must have max_node of 2 or more.\n");
		tp_assert(0, "max_node error");
	}

	/* mbind and tpassert(mbind) */
	l_ret = mbind(buf, PAGE_SIZE, MPOL_DEFAULT, &mbind_node_mask, 0, 0);
	tp_assert(l_ret==-1, "mbind not failed");
	tp_assert(errno==EINVAL, "errno != EINVAL");

	/* result OK */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
