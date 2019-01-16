/* 018.c COPYRIGHT FUJITSU LIMITED 2017 */
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

#define MAX_COUNT 3

static unsigned long max_node;
static unsigned long *node_mask = NULL;
static unsigned long nr_node_mask;

static char *buf = MAP_FAILED;
static char *reserve = MAP_FAILED;
static char *addr = MAP_FAILED;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
        /* arg analyze and get max_node */
	max_node = arg_analyze(tc_argc, tc_argv);
        return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int mode = -1, old_mode;
	int i_ret = -1;
	long l_ret = -1;
	int i;
	char *addr;
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
	l_ret = set_mempolicy(MPOL_INTERLEAVE, &set_node_mask, BITS_PER_LONG);
	tp_assert(l_ret==0, "set_mempolicy error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	mode = -1;
	i_ret = -1;
	i_ret = get_mempolicy(&mode, node_mask, max_node, NULL, 0);
	tp_assert(i_ret==0, "get_mempolicy(flags=0) error");
	tp_assert(mode==MPOL_INTERLEAVE, "mode != MPOL_INTERLEAVE");
	tp_assert(node_mask[0]==set_node_mask, "set_node_mask error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	mode = -1;
	i_ret = -1;
	i_ret = get_mempolicy(&mode, NULL, 0, NULL, MPOL_F_NODE);
	tp_assert(i_ret==0, "get_mempolicy(flags=MPOL_F_NODE) error");
	old_mode = mode;

	for (i = 0; i < MAX_COUNT; i++) {
		//printf("count = %d\n", i);

		/*
		 * Since the node ID was not switched 
		 * when allocating a small size,
		 * allocate a large page size.
		 */

		/* mmap */
		reserve = (char *)mmap(NULL, LARGE_PAGE_SIZE * 2, PROT_NONE, 
					 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		tp_assert(reserve!=MAP_FAILED, "mmap(reserve) error");

		/* LARGE_PAGE_SIZE align */
		addr = (char *)(((unsigned long)reserve + LARGE_PAGE_SIZE - 1) & ~(LARGE_PAGE_SIZE - 1));

		/* mmap */
		buf = (char *)mmap(addr, LARGE_PAGE_SIZE, PROT_READ | PROT_WRITE, 
					 MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		tp_assert(buf!=MAP_FAILED, "mmap(buf) error");

		/* use a pointer to free. */
		addr = buf + LARGE_PAGE_SIZE;

		/* write */
		*buf = i;
		
		/* get_mempolicy and tp_assert(get_mempolicy) */
		mode = -1;
		i_ret = -1;
		i_ret = get_mempolicy(&mode, NULL, 0, NULL, MPOL_F_NODE);
		tp_assert(i_ret==0, "get_mempolicy(flags=MPOL_F_NODE) error");

		/* tp_assert (mode!=old_mode) */
		tp_assert(mode != old_mode, "mode == old_mode");
		old_mode = mode;

		/* free */
		munmap(reserve, buf - reserve);
		munmap(buf, LARGE_PAGE_SIZE);
		munmap(addr, reserve+(LARGE_PAGE_SIZE*2) - addr);
		reserve = MAP_FAILED;
		buf = MAP_FAILED;
		addr = MAP_FAILED;
	}

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* free nodemask */
	destroy_node_mask(node_mask);

	/* munmap */
	if (reserve != MAP_FAILED) {
		munmap(reserve, buf - reserve);
	}
	if (buf != MAP_FAILED) {
		munmap(buf, LARGE_PAGE_SIZE);
	}
	if (addr != MAP_FAILED) {
		munmap(addr, reserve+(LARGE_PAGE_SIZE*2) - addr);
	}
}

