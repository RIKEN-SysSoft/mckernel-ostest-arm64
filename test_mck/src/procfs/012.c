/* 012.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "test_mck.h"
#include "testsuite.h"

static int procfd = -1;
static int devfd = -1;
static char* devmap = MAP_FAILED;

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int status;
	char buf[1];

	/* open */
	devfd = open("/dev/test_mck/mmap_dev2", O_RDWR);
	tp_assert(devfd != -1, "open mmap_dev2 error.");

	/* ioctl */
	status = ioctl(devfd, TEST_MCK_MMAP_DEV2_BUF_RESET);
	tp_assert(status != -1, "ioctl error.");

	/* mmap */
	devmap = (char*)mmap((caddr_t)0, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, devfd, 0);
	tp_assert(devmap != MAP_FAILED, "mmap error");

	/* accsess */
	*devmap = 'Z';

	/* open */
	procfd = open("/proc/self/mem", O_RDONLY);
	tp_assert(procfd != -1, "open proc mem error.");

	/* seek */
	status = lseek(procfd, (off_t)devmap, SEEK_SET);
	tp_assert(status != -1, "lseek error.");

	/* read */
	status = read(procfd, buf, sizeof(buf)/sizeof(buf[0]));
	tp_assert(status == -1, "read success.");
	tp_assert(errno == EIO, "invalid errno.");
	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (procfd != -1) {
		close(procfd);
	}

	if (devmap != MAP_FAILED) {
		munmap(devmap, PAGE_SIZE);
	}

	if (devfd != -1) {
		close(devfd);
	}
}
