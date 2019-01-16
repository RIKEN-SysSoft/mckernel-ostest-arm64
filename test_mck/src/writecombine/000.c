/* 000.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "test_mck.h"

struct writecombine_args {
	const char *path;
};

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct writecombine_args args;
	int opt;

	/*
 	 * d: device file path
 	 */
	while ((opt = getopt(tc_argc, tc_argv, "d:")) != -1) {
		switch (opt) {
		case 'd':
			args.path = optarg;
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int fd = -1;
	char *buf = NULL;
	char *ret = NULL;
	struct writecombine_args *args = (struct writecombine_args *)tc_arg;

	tp_assert(args != NULL, "internal error");
	tp_assert(args->path != NULL, "invalid argument(-d).");

	fd = open(args->path, O_RDWR);
	if (fd == -1) {
		perror("open() device file");
		ret = "open() error.";
		goto out;
	}

	buf = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED) {
		perror("mmap()");
		ret = "mmap() error.";
		goto out;
	}
	*buf = 'Z';

out:
	if (buf) {
		munmap(buf, PAGE_SIZE);
	}

	if (fd != -1) {
		close(fd);
	}
	return ret;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
