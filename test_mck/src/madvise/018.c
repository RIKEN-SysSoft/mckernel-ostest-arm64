/* 018.c COPYRIGHT FUJITSU LIMITED 2018 */
#include "test_mck.h"
#include "testsuite.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct madvise_args* args;
	args = madvise_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct madvise_args* args;
	unsigned long *buf = NULL;
	char *nullp = NULL;
	int ret = 0, i = 0, fd = 0;
	unsigned long *page1, *page2, *page3;

	args = tc_arg;
	tp_assert(args->file != NULL, "invalid -f option.");

	/* open */
	fd = open(args->file, O_RDWR | O_CREAT);
	tp_assert(fd != -1, "file open error.");

	buf = (unsigned long *)mmap(0, 3 * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	tp_assert(buf != MAP_FAILED, "mmap error.");

	for (i = 0; i < 3 * PAGE_SIZE / sizeof(unsigned long); i++) {
		buf[i] = i;
	}

	/* center PAGE setting MADV_DONTDUMP. */
	ret = madvise((char *)buf + PAGE_SIZE, PAGE_SIZE, MADV_DONTDUMP);
	if (ret != 0) {
		printf("madvise failed (errno=%d)\n", errno);
		return "NG.";
	}

	page1 = buf;
	page2 = (void *)(unsigned long)buf + PAGE_SIZE;
	page3 = (void *)(unsigned long)page2 + PAGE_SIZE;

	/* coredump */
	*nullp = 0;

	munmap(buf, 3 * PAGE_SIZE);

	close(fd);

	/* warning fix */
	buf = page1;
	buf = page2;
	buf = page3;

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
