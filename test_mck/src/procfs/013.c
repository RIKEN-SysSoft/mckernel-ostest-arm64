/* 013.c COPYRIGHT FUJITSU LIMITED 2018 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "test_mck.h"
#include "testsuite.h"

#define PROCFILE_LEN 256

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd = 0;
	pid_t pid = getpid();
	char pfname[PROCFILE_LEN];
	unsigned long buf;

	/* procfile name generate */
	snprintf(pfname, sizeof(pfname), "/proc/%d/pagemap", pid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto open_err;
	}

	/* read */
	if ((read(fd, &buf, sizeof(buf))) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("dump %s(offset:0x00):%#016lx\n", pfname, buf);

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto close_err;
	}

	/* check contents */
#define PFN_MASK ((1UL << 55) - 1)
	tp_assert((buf & (1UL << 63)) == 0, "present bit is not set");
	tp_assert(((buf >> 55) & 0x3f) == 16, "page shift is 16");
	tp_assert((buf & PFN_MASK) == 0, "PFN is zero");

	//tp_assert(0, "you need check McKernel Log & Dump PAGEMAP.");
	return NULL;

/* error case */
read_err:
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
	}

open_err:
close_err:
	tp_assert(0, "TP failed.");
}
