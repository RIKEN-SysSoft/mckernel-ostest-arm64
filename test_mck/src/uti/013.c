/* 013.c COPYRIGHT FUJITSU LIMITED 2018 */
/* util_indicate_clone use pthread_create */
#include <pthread.h>
#include "test_mck.h"
#include "testsuite.h"

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)
TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)

static void *child_thread_local(void *arg)
{
	int ret;

	ret = get_system();
	if (ret == 0) {
		printf("[OK] this thread running on McKernel.\n");
	} else if (ret == -1) {
		printf("[NG] this thread running on HOST-Linux.\n");
	} else {
		printf("[NG] get_system illegal return value(%d)\n", ret);
	}
	return NULL;
}

static void *child_thread_remote(void *arg)
{
	int ret;

	ret = get_system();
	if (ret == 0) {
		printf("[NG] this thread running on McKernel.\n");
	} else if (ret == -1) {
		printf("[OK] this thread running on HOST-Linux.\n");
	} else {
		printf("[NG] get_system illegal return value(%d)\n", ret);
	}
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret = 0;
	uti_attr_t attr;
	pthread_t thread;

	/* for remote spawn */
	memset(&attr, 0, sizeof(attr));
	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_indicate_clone(SPAWN_TO_REMOTE, &attr);
	if (ret != 0) {
		perror("util_indicate_clone:");
		tp_assert(0, "util_indicate_clone error.");
	}

	memset(&thread, 0, sizeof(thread));
	ret = pthread_create(&thread, NULL, child_thread_remote, NULL);
	tp_assert(ret == 0, "remote pthread_create error.");

	ret = pthread_join(thread, NULL);
	tp_assert(ret == 0, "remote pthread_join error.");

	/* for local spawn */
	memset(&attr, 0, sizeof(attr));
	attr.flags |= UTI_FLAG_SAME_NUMA_DOMAIN;
	ret = util_indicate_clone(SPAWN_TO_LOCAL, &attr);
	if (ret != 0) {
		perror("util_indicate_clone:");
		tp_assert(0, "util_indicate_clone error.");
	}

	memset(&thread, 0, sizeof(thread));
	ret = pthread_create(&thread, NULL, child_thread_local, NULL);
	tp_assert(ret == 0, "local pthread_create error.");

	ret = pthread_join(thread, NULL);
	tp_assert(ret == 0, "local pthread_join error.");

	tp_assert(0, "please check logs. [OK] or [NG]");
	return NULL;
}
