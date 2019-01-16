/* 027.c COPYRIGHT FUJITSU LIMITED 2017 */
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
	long l_ret = -1;
	struct mbind_args m_args[] = {
		{ MPOL_BIND, 0x02, 0, 1 * PAGE_SIZE, },
		{ MPOL_BIND, 0x01, 0, 1 * PAGE_SIZE, }
	};
	buf = (char *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
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

	m_args[0].addr = buf;
	m_args[1].addr = buf;

	/* ---------- mbind and get ---------- */
 
	/*  This TP congirms that if MPOL_MF_MOVE is specifed for 
	 *  flags and mode is not MPOL_DEFAULT, it a page exists in
	 *  a memory area not following the specified policy,
	 *  it will be error EIO.
	 *  
	 *  The assumed behavior is to fail with EIO
	 *  because it mbinds to secure a physical page, 
	 *  mpinds MPOL_MF_STRICT to that area, 
	 *  mbinds with specifying a different policy, 
	 *  and does not follow the specified policy.
	 *  This is because even if flags = 0,
	 *  it is migreated.
	 */

	/* mbind and tpassert(mbind) */
	l_ret = -1;
	l_ret = mbind(m_args[0].addr, m_args[0].len,
			m_args[0].mode, &m_args[0].node_mask, BITS_PER_LONG, 0);
	tp_assert(l_ret==0, "mbind(flags=0) error");

	*buf = 0;

	/* mbind and tpassert(mbind) */
	l_ret = -1;
	l_ret = mbind(m_args[1].addr, m_args[1].len,
			m_args[1].mode, &m_args[1].node_mask, BITS_PER_LONG, MPOL_MF_STRICT);
	tp_assert(l_ret==-1, "mbind(flags=MPOL_MF_STRICT) not failed");
	tp_assert(errno!=EIO, "errno != EIO");

	munmap(buf, PAGE_SIZE);
	buf = MAP_FAILED;

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free node_mask */
	destroy_node_mask(node_mask);

	if (buf != MAP_FAILED) {
		munmap(buf, PAGE_SIZE);
	}
}
