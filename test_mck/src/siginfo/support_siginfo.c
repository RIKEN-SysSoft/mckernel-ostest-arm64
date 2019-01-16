/* support_siginfo.c COPYRIGHT FUJITSU LIMITED 2015-2017 */

#include "test_mck.h"
#include "testsuite.h"
#include <errno.h>

unsigned int delivered_signal = 0;

static void siginfo_handler(int sig, siginfo_t *sip, void *ucp)
{
	handle_siginfo[sig] = *sip;
	delivered_signal |= (1UL << sig);
}

int setup_siginfo_handler(void)
{
	int result = 0;
	int signum;

	struct sigaction sa;
	sa.sa_sigaction = siginfo_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	for(signum = 1; signum < ARCH_S64FX_SIGRTMIN; signum++){
		if(signum == SIGKILL || signum == SIGSTOP) {
			continue;
		} else {
			result = sigaction(signum, &sa, NULL);
		}

		/* There is something wrong? */
		if(result != 0) break;
	}

	return result;
}

void print_catch_siginfo(unsigned int sig_mask)
{
	static unsigned int print_ds = 0;
	int sig;
	int ret = 0;

	for (sig = 1; sig < ARCH_S64FX_SIGRTMIN; sig++) {
		if (sig_mask & (1UL << sig)) {
			ret = printf("Catch signal #%d\n", sig);
			if (ret < 0) {
				printf("printf error. (ret=%d, errno=%d)\n", ret, errno);
			}

			ret = printf("  siginfo->si_signo = %d\n", handle_siginfo[sig].si_signo);
			if (ret < 0) {
				printf("printf error. (ret=%d, errno=%d)\n", ret, errno);
			}

			ret = printf("  siginfo->si_errno = %d\n", handle_siginfo[sig].si_errno);
			if (ret < 0) {
				printf("printf error. (ret=%d, errno=%d)\n", ret, errno);
			}

			ret = printf("  siginfo->si_code  = 0x%x\n", handle_siginfo[sig].si_code);
			if (ret < 0) {
				printf("printf error. (ret=%d, errno=%d)\n", ret, errno);
			}
			print_ds |= (1UL << sig);
			printf("delivered_signal=0x%08x\n", print_ds);
		}
	}
}
