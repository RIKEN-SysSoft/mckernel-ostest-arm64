/* common.h COPYRIGHT FUJITSU LIMITED 2018 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ihklib.h>

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

extern void print_test_overview(int tp_num);
extern int get_status(int os_num);
extern int event_polling(int os_num, int type);

#endif /* !__COMMON_H__ */
