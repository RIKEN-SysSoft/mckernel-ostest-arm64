/* uti.c COPYRIGHT FUJITSU LIMITED 2018 */
#include "testsuite.h"
#include <unistd.h>
#include <sys/syscall.h>

int util_migrate_inter_kernel(uti_attr_t *attr)
{
	return (int)syscall(__NR_util_migrate_inter_kernel, attr);
}

int util_indicate_clone(int mod, uti_attr_t *attr)
{
	return (int)syscall(__NR_util_indicate_clone, mod, attr);
}

int get_system(void)
{
	return (int)syscall(__NR_get_system);
}

int futex(int *uaddr, int op, int val, const struct timespec *timeout,
	  int *uaddr2, int val3)
{
	return (int)syscall(__NR_futex, uaddr, op, val, timeout, uaddr2, val3);
}
