/* main.c COPYRIGHT FUJITSU LIMITED 2017-2018 */
#include "common.h"

/* usage messages */
const char *usage_messages[] = {
/* TP# 0 */	"set/enable/get/disable/destroy check config(0x00-0x3f). (for each)",
/* TP# 1 */	"set/enable/get/disable/destroy check config(0x00-0x3f). (all at once)",
/* TP# 2 */	"set/enable/get/disable/destroy check disabled flags on.",
/* TP# 3 */	"set/enable/get/disable/destroy check pinned flags on.",
/* TP# 4 */	"set/enable/get/disable/destroy check exclude flags on.",
/* TP# 5 */	"set/enable/get/disable/destroy check config(0x00-0x07). (8 counter at once)",
};

struct test_case {
	int (*func)(int, int);
};

#define TEST_CASE_DEF(number)	\
	extern int test_##number(int, int);
#include "test_case.list"
#undef TEST_CASE_DEF

#define TEST_CASE_DEF(number) { .func = test_##number },
const struct test_case test_cases[] = {
#include "test_case.list"
};
#undef TEST_CASE_DEF

static void usage(char *string)
{
	int i = 0;
	int tp_count = sizeof(usage_messages) / sizeof(usage_messages[0]);

	printf("%s <os_num> <tp_num>\n", string);
	printf("   os_num    : os number.\n");
	printf("   tp_num    : tp number. (0-%d)\n", tp_count - 1);

	for (i = 0; i < tp_count; i++) {
		printf("%6d : %s\n", i, usage_messages[i]);
	}
}

int main(int argc, char *argv[])
{
	int ret = -1;
	int os_num = 0;
	int tp_num = 0;
	const int usage_count = sizeof(usage_messages) / sizeof(usage_messages[0]);
	const int tp_count = sizeof(test_cases) / sizeof(test_cases[0]);

	if (usage_count != tp_count) {
		printf("BUG: Usage strings and TP count are different.\n");
		goto out;
	}

	if (argc != 3) {
		usage(argv[0]);
		goto out;
	}

	os_num = atoi(argv[1]);
	if (os_num < 0 || 63 < os_num) {
		printf("Invalid argument 1 os_num.\n");
		usage(argv[0]);
		goto out;
	}

	tp_num = atoi(argv[2]);
	if (tp_count <= tp_num) {
		printf("TP number invaid.\n");
		usage(argv[0]);
		goto out;
	}

	if (test_cases[tp_num].func) {
		ret = test_cases[tp_num].func(tp_num, os_num);
	} else {
		printf("BUG: TP function pointer is NULL.\n");
	}

	if (ret) {
		printf("RESULT:NG.\n");
	} else {
		printf("RESULT:OK.\n");
	}
out:
	return ret;
}
