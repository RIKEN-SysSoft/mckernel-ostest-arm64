/* 000.c COPYRIGHT FUJITSU LIMITED 2016-2017 */
#include "test_mck.h"
#include "testsuite.h"
#include <unistd.h>
#include <string.h>

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	sigset_t set;

	/* signal masking */
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);	/* ITIMER_REAL */
	sigaddset(&set, SIGVTALRM);	/* ITIMER_VIRTUAL */
	sigaddset(&set, SIGPROF);	/* ITIMER_PROF */
	sigprocmask(SIG_BLOCK, &set, NULL);
	return NULL;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int result;
	const time_t init_sec = 1000;
	const suseconds_t init_usec = 100;
	struct itimerval old_value;
	struct itimerval new_value;
	struct itimerval reset_value;

	int itimer_type[] = {
		ITIMER_REAL,
		ITIMER_VIRTUAL,
		ITIMER_PROF,
	};
	char* itimer_name[] = {
		"ITIMER_REAL",
		"ITIMER_VIRTUAL",
		"ITIMER_PROF",
	};

	for (i = 0; i < sizeof(itimer_type)/sizeof(itimer_type[0]); ++i)
	{
		memset(&old_value, 0, sizeof(old_value));
		memset(&new_value, 0, sizeof(new_value));
		memset(&reset_value, 0, sizeof(reset_value));

		new_value.it_value.tv_sec = init_sec;
		new_value.it_value.tv_usec = init_usec;

		result = setitimer(itimer_type[i], &new_value, NULL);
		tp_assert(result == 0, "setitimer(1st) failed.");

		printf("testing %s, please wait...\n", itimer_name[i]);
		while (1) {
			result = getitimer(itimer_type[i], &old_value);
			tp_assert(result == 0, "getitimer failed.");

			if (old_value.it_value.tv_sec < init_sec) {
				break;
			}
			cpu_pause();
		}
		result = setitimer(itimer_type[i], &reset_value, &old_value);
		tp_assert(result == 0, "setitimer(2nd) failed.");

		printf("old sec:%ld usec:%ld, init sec:%ld usec:%ld\n",
			old_value.it_value.tv_sec, old_value.it_value.tv_usec, init_sec, init_usec);
	}
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
