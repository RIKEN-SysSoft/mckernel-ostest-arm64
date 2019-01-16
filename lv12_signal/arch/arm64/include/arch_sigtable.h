/* arch_sigtable.h COPYRIGHT FUJITSU LIMITED 2015 */
#ifndef __ARM64_ARCH_SIGTABLE_H
#define __ARM64_ARCH_SIGTABLE_H

#include <signal.h>
/* for sparc signal name list */
static char *sig_name[] = {
	"DUMMY",								/*  0, dummy */
	"SIGHUP",           "SIGINT",  "SIGQUIT",  "SIGILL",  "SIGTRAP",	/*  1- 5 */
	"SIGABRT / SIGIOT", "SIGBUS",  "SIGFPE",   "SIGKILL", "SIGUSR1",	/*  6-10 */
	"SIGSEGV",          "SIGUSR2", "SIGPIPE",  "SIGALRM", "SIGTERM",	/* 11-15 */
	"SIGSTKFLT",        "SIGCHLD", "SIGCONT",  "SIGSTOP", "SIGTSTP",	/* 16-20 */
	"SIGTTIN",          "SIGTTOU", "SIGURG",   "SIGXCPU", "SIGXFSZ",	/* 21-25 */
	"SIGVTALRM",        "SIGPROF", "SIGWINCH", "SIGIO",   "SIGPWR",		/* 26-30 */
	"SIGSYS"								/* 31 */
};

/* for sparc signal num list */
static int sig_num[] = {
	SIGHUP,    SIGINT,  SIGQUIT,  SIGILL,  SIGTRAP,			/*  1- 5 */
	SIGABRT,   SIGIOT,  SIGBUS,   SIGFPE,  SIGKILL, SIGUSR1,	/*  6-10(SIGABRT==SIGIOT) */
	SIGSEGV,   SIGUSR2, SIGPIPE,  SIGALRM, SIGTERM,			/* 11-15 */
	SIGSTKFLT, SIGCHLD, SIGCONT,  SIGSTOP, SIGTSTP,			/* 16-20 */
	SIGTTIN,   SIGTTOU, SIGURG,   SIGXCPU, SIGXFSZ,			/* 21-25 */
	SIGVTALRM, SIGPROF, SIGWINCH, SIGIO,   SIGPWR,			/* 26-30 */
	SIGSYS								/* 31 */
};

#endif /* __ARM64_ARCH_SIGTABLE_H */
