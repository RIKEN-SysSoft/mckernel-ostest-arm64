/* arch_test_mck.h COPYRIGHT FUJITSU LIMITED 2015-2017 */
#ifndef __ARM64_ARCH_TEST_MCK_H
#define __ARM64_ARCH_TEST_MCK_H

/* config */
//#define CONFIG_ARM64_VA_BITS        39
#define CONFIG_ARM64_PGTABLE_LEVELS 3
#define CONFIG_ARM64_VA_BITS        48
//#define CONFIG_ARM64_PGTABLE_LEVELS 4
#define VA_BITS                     CONFIG_ARM64_VA_BITS

/* auxv */
extern char *auxv_name[];

/* memory */
#if !defined(CONFIG_ARM64_64K_PAGES)
# define PAGE_SHIFT        12 //4KB
#elif 0
# error 16KB granule size not supported.
# define PAGE_SHIFT        14 //16KB
#else
# define PAGE_SHIFT        16 //64KB
#endif

#define LARGE_PAGE_SHIFT	(PAGE_SHIFT + 9 + (PAGE_SHIFT - 12))
#define LARGEST_PAGE_SHIFT	(LARGE_PAGE_SHIFT + 9 + (PAGE_SHIFT - 12))
#if PAGE_SHIFT == 12
# define ENABLE_LARGEST_PAGE 1
#else
# define ENABLE_LARGEST_PAGE 0
#endif

#if (VA_BITS == 39 && PAGE_SHIFT == 12)
# define MMAP_START_ADDR 0x0000000800000000UL /* in McKernel TASK_UNMAPPED_BASE value */
# define MMAP_END_ADDR   0x0000002000000000UL /* in McKernel USER_END value */
#elif (VA_BITS == 42 && PAGE_SHIFT == 16)
# define MMAP_START_ADDR 0x0000004000000000UL /* in McKernel TASK_UNMAPPED_BASE value */
# define MMAP_END_ADDR   0x0000010000000000UL /* in McKernel USER_END value */
#elif (VA_BITS == 48 && PAGE_SHIFT == 12)
# define MMAP_START_ADDR 0x0000100000000000UL /* in McKernel TASK_UNMAPPED_BASE value */
# define MMAP_END_ADDR   0x0000400000000000UL /* in McKernel USER_END value */
#elif (VA_BITS == 48 && PAGE_SHIFT == 16)
# define MMAP_START_ADDR 0x0000100000000000UL /* in McKernel TASK_UNMAPPED_BASE value */
# define MMAP_END_ADDR   0x0000400000000000UL /* in McKernel USER_END value */
#else
# error virtual address space not defined.
#endif
#define MMAP_AREA_SIZE	(MMAP_END_ADDR - MMAP_START_ADDR)

/* asm code */
#define get_stack_addr()						\
	({								\
		unsigned long sp = 0;					\
		__asm__ __volatile__("mov %0, sp \n"			\
				     : "=r"(sp)				\
				     :					\
				     : "memory");			\
		sp;							\
	})

#define cpu_pause()						\
	({							\
		__asm__ __volatile__("yield" ::: "memory");	\
	})

#define register_write()				\
             asm volatile (				\
     "       mov     x0,  0x0000000000000000 \n"	\
     "       mov     x1,  0x0000000000000001 \n"	\
     "       mov     x2,  0x0000000000000002 \n"	\
     "       mov     x3,  0x0000000000000003 \n"	\
     "       mov     x4,  0x0000000000000004 \n"	\
     "       mov     x5,  0x0000000000000005 \n"	\
     "       mov     x6,  0x0000000000000006 \n"	\
     "       mov     x7,  0x0000000000000007 \n"	\
     "       mov     x8,  0x0000000000000008 \n"	\
     "       mov     x9,  0x0000000000000009 \n"	\
     "       mov     x10, 0x000000000000000a \n"	\
     "       mov     x11, 0x000000000000000b \n"	\
     "       mov     x12, 0x000000000000000c \n"	\
     "       mov     x13, 0x000000000000000d \n"	\
     "       mov     x14, 0x000000000000000e \n"	\
     "       mov     x15, 0x000000000000000f \n"	\
     "       mov     x16, 0x0000000000000010 \n"	\
     "       mov     x17, 0x0000000000000011 \n"	\
     "       mov     x18, 0x0000000000000012 \n"	\
     "       mov     x19, 0x0000000000000013 \n"	\
     "       mov     x20, 0x0000000000000014 \n"	\
     "       mov     x21, 0x0000000000000015 \n"	\
     "       mov     x22, 0x0000000000000016 \n"	\
     "       mov     x23, 0x0000000000000017 \n"	\
     "       mov     x24, 0x0000000000000018 \n"	\
     "       mov     x25, 0x0000000000000019 \n"	\
     "       mov     x26, 0x000000000000001a \n"	\
     "       mov     x27, 0x000000000000001b \n"	\
     "       mov     x28, 0x000000000000001c \n"	\
            );						\

#define Op0_shift      19
#define Op0_mask       0x3
#define Op1_shift      16
#define Op1_mask       0x7
#define CRn_shift      12
#define CRn_mask       0xf
#define CRm_shift      8
#define CRm_mask       0xf
#define Op2_shift      5
#define Op2_mask       0x7

#define sys_reg(op0, op1, crn, crm, op2)	       \
	(((op0) << Op0_shift) | ((op1) << Op1_shift) | \
	 ((crn) << CRn_shift) | ((crm) << CRm_shift) | \
	 ((op2) << Op2_shift))

#define __stringify_1(x...) #x
#define __stringify(x...)   __stringify_1(x)

#ifdef __ASSEMBLY__
#define __emit_inst(x)			.inst (x)
#else
#define __emit_inst(x)			".inst " __stringify((x)) "\n\t"
#endif

#ifdef __ASSEMBLY__

	.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	.equ	.L__reg_num_x\num, \num
	.endr
	.equ	.L__reg_num_xzr, 31

	.macro	mrs_s, rt, sreg
	 __emit_inst(0xd5200000|(\sreg)|(.L__reg_num_\rt))
	.endm

	.macro	msr_s, sreg, rt
	__emit_inst(0xd5000000|(\sreg)|(.L__reg_num_\rt))
	.endm

#else /* __ASSEMBLY__ */

	asm(
		"	.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30\n"
		"	.equ	.L__reg_num_x\\num, \\num\n"
		"	.endr\n"
		"	.equ	.L__reg_num_xzr, 31\n"
		"\n"
		"	.macro	mrs_s, rt, sreg\n"
		__emit_inst(0xd5200000|(\\sreg)|(.L__reg_num_\\rt))
		"	.endm\n"
		"\n"
		"	.macro	msr_s, sreg, rt\n"
		__emit_inst(0xd5000000|(\\sreg)|(.L__reg_num_\\rt))
		"	.endm\n"
		);

#endif /* __ASSEMBLY__ */

/* system register */
#define ID_MMFR4_EL1     sys_reg(3,0,0,2,6)
#define ID_AA64MMFR2_EL1 sys_reg(3,0,0,7,2)
#define ZCR_EL1          sys_reg(3,0,1,2,0)
#define ID_AA64ZFR0_EL1  sys_reg(3,0,0,4,4)

#endif /* __ARM64_ARCH_TEST_MCK_H */

/* syscall number */
#ifndef __NR_getcpu
# define __NR_getcpu 168
#endif

#ifndef __NR_process_vm_readv
# define __NR_process_vm_readv 270
#endif

#ifndef __NR_process_vm_writev
# define __NR_process_vm_writev 271
#endif
