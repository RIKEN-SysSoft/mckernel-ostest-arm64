/* 012.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include "test_mck.h"
#include "testsuite.h"

static int devfd = -1;
static char* devmap = MAP_FAILED;
static size_t devmap_size = 0;

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	return sched_setaffinity_parse_args(tc_argc, tc_argv);
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	struct sched_setaffinity_args *args = (struct sched_setaffinity_args*)tc_arg;
	int result;
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus;

	tp_assert(args->num_of_cpus != 0, "mandatory parameter '-p <num_of_cpus>'");
	num_cpus = args->num_of_cpus;

	/* open */
	devfd = open("/dev/test_mck/mmap_dev2", O_RDWR);
	tp_assert(devfd != -1, "open mmap_dev2 error.");

	/* ioctl */
	result = ioctl(devfd, TEST_MCK_MMAP_DEV2_BUF_RESET);
	tp_assert(result != -1, "ioctl error.");

	/* mmap */
	size = CPU_ALLOC_SIZE(num_cpus);
	cpusetp = mmap((caddr_t)0, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, devfd, 0);
	tp_assert(cpusetp != MAP_FAILED, "mmap error");
	CPU_ZERO_S(size, cpusetp);

	devmap = (void*)cpusetp;
	devmap_size = size;

	// get own affinity
	result = sched_getaffinity(0, size, cpusetp);
	tp_assert(result != -1, "sched_getaffinity failed.");

	// rewrite own affinity
	result = sched_setaffinity(0, size, cpusetp);
	tp_assert(result != -1, "sched_setaffinity failed.");

	return NULL;
}

TEARDOWN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	if (devmap != MAP_FAILED) {
		munmap(devmap, PAGE_SIZE);
	}

	if (devfd != -1) {
		close(devfd);
	}
}
