/* arch_test_fpregs.h COPYRIGHT FUJITSU LIMITED 2016 */

#ifndef __ARM64_ARCH_TEST_FPREGS_H
#define __ARM64_ARCH_TEST_FPREGS_H

#include "test_data_sample.h"

#define FPREGS_TEST_DATA SAMPLE_TEST_DATA_512BYTES

struct fpregs {
     union  {
          __uint8_t   data[512];
          __uint64_t  ddata[64];
          __uint128_t qdata[32];
     };
};

static inline void fpregs_write(struct fpregs *regs)
{
	asm volatile(
		"ldp q0, q1, [%0, #16 * 0]\n"
		"ldp q2, q3, [%0, #16 * 2]\n"
		"ldp q4, q5, [%0, #16 * 4]\n"
		"ldp q6, q7, [%0, #16 * 6]\n"
		"ldp q8, q9, [%0, #16 * 8]\n"
		"ldp q10, q11, [%0, #16 * 10]\n"
		"ldp q12, q13, [%0, #16 * 12]\n"
		"ldp q14, q15, [%0, #16 * 14]\n"
		"ldp q16, q17, [%0, #16 * 16]\n"
		"ldp q18, q19, [%0, #16 * 18]\n"
		"ldp q20, q21, [%0, #16 * 20]\n"
		"ldp q22, q23, [%0, #16 * 22]\n"
		"ldp q24, q25, [%0, #16 * 24]\n"
		"ldp q26, q27, [%0, #16 * 26]\n"
		"ldp q28, q29, [%0, #16 * 28]\n"
		"ldp q30, q31, [%0, #16 * 30]\n"
		: "=r" (regs)
		: : "memory"
	);
}

static inline void fpregs_read(struct fpregs *regs)
{
	asm volatile(
		"stp q0, q1, [%0, #16 * 0]\n"
		"stp q2, q3, [%0, #16 * 2]\n"
		"stp q4, q5, [%0, #16 * 4]\n"
		"stp q6, q7, [%0, #16 * 6]\n"
		"stp q8, q9, [%0, #16 * 8]\n"
		"stp q10, q11, [%0, #16 * 10]\n"
		"stp q12, q13, [%0, #16 * 12]\n"
		"stp q14, q15, [%0, #16 * 14]\n"
		"stp q16, q17, [%0, #16 * 16]\n"
		"stp q18, q19, [%0, #16 * 18]\n"
		"stp q20, q21, [%0, #16 * 20]\n"
		"stp q22, q23, [%0, #16 * 22]\n"
		"stp q24, q25, [%0, #16 * 24]\n"
		"stp q26, q27, [%0, #16 * 26]\n"
		"stp q28, q29, [%0, #16 * 28]\n"
		"stp q30, q31, [%0, #16 * 30]\n"
		: : "r" (regs)
		: "memory"
	);
}

static inline void show_current_fpregs(void)
{
	struct fpregs fregs;
	int i;

	memset(&fregs, 0, sizeof(struct fpregs));
	fpregs_read(&fregs);

	for (i = 0; i < 32; i++) {
		printf("q%02d: 0x%016lx_%016lx\n", 
			i, fregs.ddata[(i*2)+1], fregs.ddata[i*2]); // swap because endianess.
	}

	return;
}

#endif /* __ARM64_ARCH_TEST_FPREGS_H */
