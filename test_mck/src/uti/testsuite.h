/* testsuite.h COPYRIGHT FUJITSU LIMITED 2018 */
#ifndef __TEST_SUITE_H__
#define __TEST_SUITE_H__

#include <string.h>
#include <stdint.h>
#include <time.h>

/* Linux default value is used */
#define UTI_MAX_NUMA_DOMAINS		(1024)

typedef struct uti_attr {
	/* UTI_CPU_SET environmental variable is used to denote the preferred
	   location of utility thread */
	uint64_t numa_set[(UTI_MAX_NUMA_DOMAINS + sizeof(uint64_t) * 8 - 1) /
			  (sizeof(uint64_t) * 8)];
	uint64_t flags; /* Representing location and behavior hints by bitmap */
} uti_attr_t;

/* struct uti_attr flags value */
#define UTI_FLAG_E			(1ULL<<0)
#define UTI_FLAG_NUMA_SET		(1ULL<<1)	/* Indicates NUMA_SET is specified */
#define UTI_FLAG_SAME_NUMA_DOMAIN	(1ULL<<2)
#define UTI_FLAG_DIFFERENT_NUMA_DOMAIN	(1ULL<<3)
#define UTI_FLAG_SAME_L1		(1ULL<<4)
#define UTI_FLAG_SAME_L2		(1ULL<<5)
#define UTI_FLAG_SAME_L3		(1ULL<<6)
#define UTI_FLAG_DIFFERENT_L1		(1ULL<<7)
#define UTI_FLAG_DIFFERENT_L2		(1ULL<<8)
#define UTI_FLAG_DIFFERENT_L3		(1ULL<<9)
#define UTI_FLAG_EXCLUSIVE_CPU		(1ULL<<10)
#define UTI_FLAG_CPU_INTENSIVE		(1ULL<<11)
#define UTI_FLAG_HIGH_PRIORITY		(1ULL<<12)
#define UTI_FLAG_NON_COOPERATIVE	(1ULL<<13)

/* util_indicate_clone mod argument value */
#define SPAWN_TO_LOCAL			0
#define SPAWN_TO_REMOTE			1

/* uti syscall number */
#define __NR_util_migrate_inter_kernel	730
#define __NR_util_indicate_clone	731
#define __NR_get_system			732

extern int util_migrate_inter_kernel(uti_attr_t *attr);
extern int util_indicate_clone(int mod, uti_attr_t *attr);
extern int get_system(void);
extern int futex(int *uaddr, int op, int val, const struct timespec *timeout,
		 int *uaddr2, int val3);

#endif /*__TEST_SUITE_H__*/
