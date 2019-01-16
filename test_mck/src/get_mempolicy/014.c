/* 014.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#include "test_mck.h"
#include "numa.h"

static char *buf = MAP_FAILED;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int mode = -1;
	int i_ret = -1;

	/* mmap */
	buf = (char *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	tp_assert(buf!=MAP_FAILED, "mmap error");

	/* get_mempolicy and tp_assert(get_mempolicy) */
	i_ret = get_mempolicy(&mode, NULL, 0, buf, MPOL_F_ADDR);
	tp_assert(i_ret==0, "get_mempolicy error");

	/* result OK */
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	/* munmap */
	if (buf != MAP_FAILED) {
		munmap(buf, PAGE_SIZE);
	}
}
