/* iemul.c COPYRIGHT FUJITSU LIMITED 2017 */
#include "test_mck.h"
#include "testsuite.h"
#include <stdio.h>

#ifdef __aarch64__
#define __print_cpu_ftr(num, id)					\
	case (num):							\
	{								\
		unsigned long __val;					\
		asm("mrs %0, " STRING(id) : "=r" (__val));		\
		printf("%-20s: 0x%016lx\n", #id, __val);		\
		msg = NULL;						\
	}								\
	break;
#define __print_cpu_ftr_s(num, id)					\
	case (num):							\
	{								\
		unsigned long __val;					\
		asm("mrs_s %0, " STRING(id) : "=r" (__val));		\
		printf("%-20s: 0x%016lx\n", #id, __val);		\
		msg = NULL;						\
	}								\
	break;
#else /* __aarch64__ */
#define __print_cpu_ftr(num, id)   case (num): break;
#define __print_cpu_ftr_s(num, id) case (num): break;
#endif /* __aarch64__ */

const char* print_cpu_ftr(int num)
{
	const char* msg = "invalid register number.";
	switch (num) {
		__print_cpu_ftr(0, MIDR_EL1);
		__print_cpu_ftr(1, MPIDR_EL1);
		__print_cpu_ftr(2, REVIDR_EL1);
		__print_cpu_ftr(3, ID_PFR0_EL1);
		__print_cpu_ftr(4, ID_PFR1_EL1);
		__print_cpu_ftr(5, ID_DFR0_EL1);
		__print_cpu_ftr(6, ID_MMFR0_EL1);
		__print_cpu_ftr(7, ID_MMFR1_EL1);
		__print_cpu_ftr(8, ID_MMFR2_EL1);
		__print_cpu_ftr(9, ID_MMFR3_EL1);
		__print_cpu_ftr(10, ID_ISAR0_EL1);
		__print_cpu_ftr(11, ID_ISAR1_EL1);
		__print_cpu_ftr(12, ID_ISAR2_EL1);
		__print_cpu_ftr(13, ID_ISAR3_EL1);
		__print_cpu_ftr(14, ID_ISAR4_EL1);
		__print_cpu_ftr(15, ID_ISAR5_EL1);
		__print_cpu_ftr_s(16, ID_MMFR4_EL1);
		__print_cpu_ftr(17, MVFR0_EL1);
		__print_cpu_ftr(18, MVFR1_EL1);
		__print_cpu_ftr(19, MVFR2_EL1);
		__print_cpu_ftr(20, ID_AA64PFR0_EL1);
		__print_cpu_ftr(21, ID_AA64PFR1_EL1);
		__print_cpu_ftr(22, ID_AA64DFR0_EL1);
		__print_cpu_ftr(23, ID_AA64DFR1_EL1);
		__print_cpu_ftr(24, ID_AA64ISAR0_EL1);
		__print_cpu_ftr(25, ID_AA64ISAR1_EL1);
		__print_cpu_ftr(26, ID_AA64MMFR0_EL1);
		__print_cpu_ftr(27, ID_AA64MMFR1_EL1);
		__print_cpu_ftr_s(28, ID_AA64MMFR2_EL1);
		__print_cpu_ftr(29, CTR_EL0);
		__print_cpu_ftr(30, DCZID_EL0);
		__print_cpu_ftr(31, CNTFRQ_EL0);
		__print_cpu_ftr_s(32, ID_AA64ZFR0_EL1);
		__print_cpu_ftr_s(33, ZCR_EL1);
	}
	return msg;
}
