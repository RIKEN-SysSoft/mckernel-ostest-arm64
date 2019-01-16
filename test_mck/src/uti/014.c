/* 014.c COPYRIGHT FUJITSU LIMITED 2018 */
/* util_indicate_clone use fork */
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

static void child_proc_local(void)
{
	int ret;

	ret = get_system();
	if (ret == 0) {
		printf("[OK] this process running on McKernel.\n");
	} else if (ret == -1) {
		printf("[NG] this process running on HOST-Linux.\n");
	} else {
		printf("[NG] get_system illegal return value(%d)\n", ret);
	}
}

static void child_proc_remote(void)
{
	int ret;

	ret = get_system();
	if (ret == 0) {
		printf("[NG] this process running on McKernel.\n");
	} else if (ret == -1) {
		printf("[OK] this process running on HOST-Linux.\n");
	} else {
		printf("[NG] get_system illegal return value(%d)\n", ret);
	}
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	pid_t pid = 0;
	uti_attr_t attr;

	/* for remote spawn */
	memset(&attr, 0, sizeof(attr));
	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_indicate_clone(SPAWN_TO_REMOTE, &attr);
	if (ret != 0) {
		perror("util_indicate_clone:");
		tp_assert(0, "util_indicate_clone error.");
	}

	pid = fork();
	switch (pid) {
	case -1:
		tp_assert(0, "fork failed.");
		break;
	case 0:
		child_proc_remote();
		exit(0);
		break;
	default:
		waitpid(pid, NULL, 0);
		break;
	}

	/* for local spawn */
	memset(&attr, 0, sizeof(attr));
	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_indicate_clone(SPAWN_TO_LOCAL, &attr);
	if (ret != 0) {
		perror("util_indicate_clone:");
		tp_assert(0, "util_indicate_clone error.");
	}

	pid = fork();
	switch (pid) {
	case -1:
		tp_assert(0, "fork failed.");
		break;
	case 0:
		child_proc_local();
		exit(0);
		break;
	default:
		waitpid(pid, NULL, 0);
		break;
	}

	tp_assert(0, "please check logs. [OK] or [NG]");
	return NULL;
}
