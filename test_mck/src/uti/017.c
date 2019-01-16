/* 017.c COPYRIGHT FUJITSU LIMITED 2018 */
/* Confirmation of failure of specific syscall from HOST-Linux thread */
#include "test_mck.h"
#include "testsuite.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	long lret = 0;
	uti_attr_t attr;

	memset(&attr, 0, sizeof(attr));

	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_migrate_inter_kernel(&attr);
	if (ret != 0) {
		perror("util_migrate_inter_kernel:");
		tp_assert(0, "util_migrate_inter_kernel error.");
	}

	ret = get_system();
	if (ret == 0) {
		printf("this process running on McKernel.\n");
		tp_assert(0, "migrate failed.");
	} else if (ret != -1) {
		printf("get_system illegal return value(%d)\n", ret);
		tp_assert(0, "get_system error.");
	}

	/* __NR_clone */
	lret = syscall(__NR_clone);
	tp_assert(lret == -1, "clone syscall not failed.");
	tp_assert(errno == ENOSYS, "clone syscall errno is not -ENOSYS");

#ifdef __NR_fork
	/* __NR_fork */
	lret = syscall(__NR_fork);
	tp_assert(lret == -1, "fork syscall not failed.");
	tp_assert(errno == ENOSYS, "fork syscall errno is not -ENOSYS");
#endif

#ifdef __NR_vfork
	/* __NR_vfork */
	lret = syscall(__NR_vfork);
	tp_assert(lret == -1, "vfork syscall not failed.");
	tp_assert(errno == ENOSYS, "vfork syscall errno is not -ENOSYS");
#endif

	/* __NR_execve */
	lret = syscall(__NR_execve);
	tp_assert(lret == -1, "execve syscall not failed.");
	tp_assert(errno == ENOSYS, "execve syscall errno is not -ENOSYS");

	return NULL;
}
