/* 008.c COPYRIGHT FUJITSU LIMITED 2015-2017 */
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
#define BUF_LEN 256

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd = 0;
	pid_t pid = getpid();
	char pfname[PROCFILE_LEN];
	char *buf1 = NULL;
	char *aligned_buf1 = NULL;
	unsigned long buf2;
	off_t offset = 0;
	off_t ret = 0;
	char buf3;

	/* allocate */
	buf1 = calloc(PAGE_SIZE * 2, 1);
	if (buf1 == NULL) {
		tp_assert(0, "calloc failed.");
	}
	aligned_buf1 = (char *)(((unsigned long)buf1 + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
	printf("allocated: %#016lx\n", (off_t)aligned_buf1);
	*aligned_buf1 = 0xff;

	/* procfile name generate */
	snprintf(pfname, sizeof(pfname), "/proc/%d/pagemap", pid);

	/* open */
	if ((fd = open(pfname, O_RDONLY)) == -1) {
		printf("open() failed. %d\n", errno);
		goto open_err;
	}

	/* calc offset */
	offset = (off_t)aligned_buf1 / PAGE_SIZE * 8;

	/* lseek */
	if ((ret = lseek(fd, offset, SEEK_SET)) == -1) {
		printf("lseek() failed. %d\n", errno);
		goto lseek_err;
	}

	/* read */
	if ((read(fd, &buf2, sizeof(buf2))) == -1) {
		printf("read() failed. %d\n", errno);
		goto read_err;
	}

	/* dump */
	printf("dump %s(offset:%#016lx):%#016lx\n", pfname, (off_t)offset, buf2);

	/* close */
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
		goto close_err;
	}

	/* check contents */
#define PFN_MASK ((1UL << 55) - 1)
	tp_assert(buf2 & (1UL << 63), "present bit is set");
	tp_assert(((buf2 >> 55) & 0x3f) == 16, "page shift is 16");
	tp_assert(buf2 & PFN_MASK, "PFN is non-zero");

	free(buf1);
	//tp_assert(0, "you need check McKernel Log & Dump PAGEMAP.");
	return NULL;

/* error case */
lseek_err:
read_err:
	if ((close(fd)) == -1) {
		printf("close() failed. %d\n", errno);
	}

open_err:
close_err:
	free(buf1);
	tp_assert(0, "TP failed.");
}
