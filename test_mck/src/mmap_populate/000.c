/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include "testsuite.h"

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct mmap_populate_args* args;
	args = mmap_populate_parse_args(tc_argc, tc_argv);
	return args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int status;
	int fd;
	struct stat sbuf;
	char* addr;
	struct mmap_populate_args* args;

	args = tc_arg;
	tp_assert(args->file != NULL, "invalid -f option.");
	tp_assert(0 <= tc_num, "invalid tp number.");

	/* open */
	fd = open(args->file, O_RDWR);
	tp_assert(fd != -1, "file open error.");

	/* get size */
	status = stat(args->file, &sbuf);
	tp_assert(status != -1, "stat error.");

	/* mmap */ 
	addr = (char*)mmap((caddr_t)0, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_POPULATE, fd, 0);
	tp_assert(addr != MAP_FAILED, "mmap error.");

	/* munmap */
	status = munmap(addr, sbuf.st_size);
	tp_assert(status != -1, "munmap error.");

	/* close */ 
	close(fd);

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
