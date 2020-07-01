/* entry-kill-single.c COPYRIGHT FUJITSU LIMITED 2015-2017 */
#include <signal.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void sig_handler(int sig_num)
{
	printf("[  OK  ] sig_num = %d received.\n", sig_num);
	if (signal(sig_num, SIG_DFL) == SIG_ERR) {
		printf("[  NG  ] signal function default failed. sig_num = %d\n", sig_num);
		return;
	}
	return;
}

int main(int argc, char** argv) {
	int sig_num = SIGILL;

	if (signal(sig_num, sig_handler) == SIG_ERR) {
		printf("[ INFO ] signal function set failed. sig_num = %d\n", sig_num);
		return 0;
	}
	if (kill(getpid(), sig_num)) {
		printf("[  NG  ] kill failed. sig_num = %d\n", sig_num);
		return 0;
	}

	printf("end of user program.\n");
	return 0; 
}
