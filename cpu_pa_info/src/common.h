/* common.h COPYRIGHT FUJITSU LIMITED 2017 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ihklib.h>
#include <linux/perf_event.h>

/* test */
#define TEST_NAME(num)		test_##num
#define TEST_ARG(arg)		arg
#define TEST_I_ARG(arg)		int TEST_ARG(arg)
#define TEST_FUNC(num, arg1)	\
	int TEST_NAME(num)(int tp_num, TEST_I_ARG(arg1))

extern const char *usage_messages[];

#define cpu_pause()						\
	({							\
		__asm__ __volatile__("yield" ::: "memory");	\
	})

extern int get_config_num(void);
extern void print_perf_config(unsigned long config);
extern void print_test_overview(int tp_num);
extern int perfevent_exec(int os_num, struct ihk_perf_event_attr *attr, const unsigned int n);

/* @ref.impl linux-v4.15-rc3 include/linux/perf/arm_pmu.h */
#define ARMV8_PMUV3_MAX_COMMON_EVENTS		0x40

/* @ref.impl linux-v4.15-rc3 arch/arm64/include/asm/perf_event.h */
#define ARMV8_PMUV3_PERFCTR_CPU_CYCLES		0x11

/* @ref.impl linux-v4.15-rc3 arch/arm64/kernel/perf_event.c */
#define ARMV8_PMUV3_PERFCTR_INST_RETIRED	0x08

#endif /* !__COMMON_H__ */
