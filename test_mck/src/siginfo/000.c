/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2017 */
#include "test_mck.h"
#include "testsuite.h"
#include <string.h>

siginfo_t handle_siginfo[ARCH_S64FX_SIGRTMIN];
SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	memset(handle_siginfo, 0, sizeof(handle_siginfo));
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int signum;
	unsigned int old_ds = 0, ds = 0;

	tp_assert(setup_siginfo_handler() == 0,
			"Setup signal handlers failed. What's happen?");

	for(signum = 1; signum < ARCH_S64FX_SIGRTMIN; signum++) {
		if(signum == SIGKILL || signum == SIGSTOP) continue;

		printf("=== raise signal #%d ===\n", signum);
		tp_assert(raise(signum) == 0, "raise signal failed");

		while(!(ds = old_ds ^ delivered_signal)) {
			cpu_pause();
		}
		old_ds |= ds;
		print_catch_siginfo(ds);
	}

	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
