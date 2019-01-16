/* 000.c COPYRIGHT FUJITSU LIMITED 2017-2018 */
#include "test_mck.h"
#include "testsuite.h"
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int ret;
	pid_t pid = getpid();

	ret = profile_proc_on();
	tp_assert(ret == 0, "profile_proc_on failed.");

	/* track target number 271 syscall */
	process_vm_writev(pid, NULL, 0, NULL, 0, 0);

	ret = profile_proc_off();
	tp_assert(ret == 0, "profile_proc_off failed.");

	ret = profile_proc_print();
	tp_assert(ret == 0, "profile_proc_print failed.");

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
