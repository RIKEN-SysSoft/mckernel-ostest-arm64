/* 019.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <numaif.h>
#include <errno.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

static unsigned long max_node;
static unsigned long *node_mask = NULL;
static unsigned long nr_node_mask;
static char *buf = MAP_FAILED;

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
	struct mbind_args m_args[] = {
		{ MPOL_INTERLEAVE, 0x03, 1, 3 * PAGE_SIZE, },
		{ MPOL_BIND,       0x01, 2, 1 * PAGE_SIZE, }
	};
	const struct mbind_expect m_expect[NUM_REGION] = {
		{ MPOL_DEFAULT,    0x00 },
		{ MPOL_INTERLEAVE, 0x03 },
		{ MPOL_BIND,       0x01 },
		{ MPOL_INTERLEAVE, 0x03 },
		{ MPOL_DEFAULT,    0x00 },
		{ MPOL_DEFAULT,    0x00 },
		{ MPOL_DEFAULT,    0x00 }
	};
	buf = (char *)mmap(NULL, PAGE_SIZE * NUM_REGION, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

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

	/* ---------- mbind and get ---------- */

	for ( i = 0; i < (sizeof(m_args) / sizeof(m_args[0])); i++ ) {
		m_args[i].addr = buf + (m_args[i].npoff * PAGE_SIZE);
		/* mbind and tpassert(mbind) */
		l_ret = -1;
		l_ret = mbind(m_args[i].addr, m_args[i].len,
				m_args[i].mode, &m_args[i].node_mask, BITS_PER_LONG, 0);
		if (l_ret != 0) {
			printf("count = %d\nerrno = %d\n", i, errno); 
			tp_assert(0, "mbind error");
		}

		/* get_mempolicy and tp_assert(get_mempolicy) */
		i_ret = -1;
		mode = -1;
		memset(node_mask, 0, sizeof(*node_mask));
		i_ret = get_mempolicy(&mode, node_mask, max_node, m_args[i].addr, MPOL_F_ADDR);
		if (i_ret != 0) {
			printf("count = %d\nerrno = %d\n", i, errno); 
			tp_assert(0, "get_mempolicy error");
		}
	}

	/* ---------- mbind check for each region ---------- */

	 /* mbind check for each region */
	i_ret = -1;
	i_ret = mbind_check(m_expect, node_mask, max_node, buf, MPOL_F_ADDR);
	tp_assert(i_ret==0, "mbind check error");	

	munmap(buf, PAGE_SIZE * NUM_REGION);
	buf = MAP_FAILED;

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free node_mask */
	destroy_node_mask(node_mask);

	if (buf != MAP_FAILED) {
		munmap(buf, PAGE_SIZE * NUM_REGION);
	}
}
