/* 003.c COPYRIGHT FUJITSU LIMITED 2016-2017 */
#include "test_mck.h"
#include "testsuite.h"

static int handled_sigalrm = 0;
static int handled_sigvtalrm = 0;
static int handled_sigaprof = 0;

static void settimer_sig_handler(int signum)
{
	switch (signum) {
		case SIGALRM:
			handled_sigalrm++;
			break;
		case SIGVTALRM:
			handled_sigvtalrm++;
			break;
		case SIGPROF:
			handled_sigaprof++;
			break;
		default:
			printf("Recived %d. Why!?\n", signum);
			break;
	}
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	signal(SIGALRM,   settimer_sig_handler); // ITIMER_REAL
	signal(SIGVTALRM, settimer_sig_handler); // ITIMER_VIRTUAL
	signal(SIGPROF,   settimer_sig_handler); // ITIMER_PROF

	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int result;
	const time_t init_time = 1; /* sec. */

	int itimer_type[] = {
		ITIMER_REAL,
		ITIMER_VIRTUAL,
		ITIMER_PROF,
	};
	char* itimer_name[] = {
		"ITIMER_REAL", "ITIMER_VIRTUAL", "ITIMER_PROF",
	};

	for (i = 0; i < sizeof(itimer_type)/sizeof(itimer_type[0]); ++i)
	{
		struct itimerval new_value = {
			{        0, 0}, /* it_interval */
			{init_time, 0}  /* it_value */
		};

		result = setitimer(itimer_type[i], &new_value, NULL);
		tp_assert(result == 0, "setitimer failed.");

		printf("testing %s, please wait...\n", itimer_name[i]);
		while(1) {
			if ((itimer_type[i] == ITIMER_REAL    && handled_sigalrm   != 0) || 
			    (itimer_type[i] == ITIMER_VIRTUAL && handled_sigvtalrm != 0) || 
			    (itimer_type[i] == ITIMER_PROF    && handled_sigaprof  != 0)) {
				break;
			}
			cpu_pause();
		}

		switch (itimer_type[i]) {
			case ITIMER_REAL:
				tp_assert(handled_sigvtalrm == 0, "Receiving unexpected signal (SIGVTALRM).");
				tp_assert(handled_sigaprof == 0, "Receiving unexpected signal (SIGPROF).");
				break;
			case ITIMER_VIRTUAL:
				tp_assert(handled_sigalrm == 0, "Receiving unexpected signal (SIGALRM).");
				tp_assert(handled_sigaprof == 0, "Receiving unexpected signal (SIGPROF).");
				break;
			case ITIMER_PROF:
				tp_assert(handled_sigalrm == 0, "Receiving unexpected signal (SIGALRM).");
				tp_assert(handled_sigvtalrm == 0, "Receiving unexpected signal (SIGVTALRM).");
				break;
		}

		// clear
		handled_sigalrm = handled_sigvtalrm = handled_sigaprof = 0;
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
