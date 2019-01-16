/* 000.c COPYRIGHT FUJITSU LIMITED 2016 */
#include "test_mck.h"

#include <signal.h>
#include <unistd.h>

static int catch_signal = 0;
static void sighandler(int sig)
{
	catch_signal = 1;
}

SETUP_EMPTY(TEST_SUITE, TEST_NUMBER)

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	sighandler_t sig_ret;
	int alarm_ret, i = 0;

	/* setting signalhandler */
	sig_ret = signal(SIGALRM, sighandler);
	tp_assert(sig_ret == SIG_DFL, "Set signal handler failed.");

	/* setting alarm 1s */
	alarm_ret = alarm(1);
	tp_assert(alarm_ret == 0, "Why already alarm setted ???");

	/* wait for event loop */
	/* event stream periodic about 100us */
	/* 10 wakeup = 1000us, less than alarm 1s */
	while (catch_signal == 0) {
		asm volatile("wfe" : : : "memory");
		i++;
	}
	printf("Wait for Event wakeup %d / s\n", i);

	/* finalize alarm */
	alarm(0);

	/* finalize signalhandler */
	signal(SIGALRM, SIG_DFL);

	return "You Need Check EventStream ON and OFF environment.";
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
