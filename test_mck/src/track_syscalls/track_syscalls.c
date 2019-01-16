/* track_syscalls.c COPYRIGHT FUJITSU LIMITED 2017-2018 */
#include "test_mck.h"
#include "testsuite.h"
#include <unistd.h>
#include <syscall.h>

static inline int profile_clear(int flag)
{
	return syscall(__NR_profile, flag | PROF_CLEAR);
}

static inline int profile_on(int flag)
{
	return syscall(__NR_profile, flag | PROF_ON);
}

static inline int profile_off(int flag)
{
	return syscall(__NR_profile, flag | PROF_OFF);
}

static inline int profile_print(int flag)
{
	return syscall(__NR_profile, flag | PROF_PRINT);
}

int profile_proc_clear(void)
{
	return profile_clear(PROF_PROC);
}

int profile_proc_on(void)
{
	return profile_on(PROF_PROC);
}

int profile_proc_off(void)
{
	return profile_off(PROF_PROC);
}

int profile_proc_print(void)
{
	return profile_print(PROF_PROC);
}

