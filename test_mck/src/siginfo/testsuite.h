/* testsuite.h COPYRIGHT FUJITSU LIMITED 2015-2016 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <signal.h>

#define ARCH_S64FX_SIGRTMIN 32

extern unsigned int delivered_signal;
extern siginfo_t handle_siginfo[];

int setup_siginfo_handler(void);
void print_catch_siginfo(unsigned int sig_mask);

#endif /*__TEST_SUITE_H__*/
