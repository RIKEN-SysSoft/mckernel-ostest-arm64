/* test_mck.c COPYRIGHT FUJITSU LIMITED 2015-2016 */
#include "test_mck.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * test suite
 */
struct test_suite {
	const char* name;
};

#define TEST_SUITE_DEF(ts) {.name = #ts},
const struct test_suite test_suites[] = {
#include "test_suite.list"
};
#undef TEST_SUITE_DEF

/*
 * test case function declaration
 */
#define TEST_CASE_DEF(ts, number)									\
	void* ts ## number ## _setup(int tp_num, int argc, char** argv);	\
	const char* ts ## number(int tp_num, void* arg);					\
	void ts ## number ## _teardown(int tp_num, void* arg);
#include "test_case.list"
#undef TEST_CASE_DEF

/*
 * test case
 */
struct test_case {
	const char* test_suite;
	int num;
	void* (*setup)(int tp_num, int argc, char** argv);
	const char* (*run)(int tp_num, void* arg);
	void (*teardown)(int tp_num, void* arg);
};

#define TEST_CASE_DEF(ts, number)				\
	{											\
		.test_suite = #ts,						\
        .num = number,                          \
		.setup = ts ## number ## _setup,		\
		.run = ts ## number,				    \
		.teardown = ts ## number ## _teardown,	\
	},
const struct test_case test_cases[] = {
#include "test_case.list"
};
#undef TEST_CASE_DEF

char* the_app;

static const char* run_test_case(const struct test_case* tc, int argc, char** argv)
{
	void* args = NULL;
	const char* msg = NULL;

	/* setup */
	args = tc->setup(tc->num, argc, argv);

	/* run */
	msg = tc->run(tc->num, args);

	/* tear_down */
	tc->teardown(tc->num, args);

	/* result */
	return msg;
}

const struct test_case* find_test_case(const char* test_suite, int num)
{
	const struct test_case* ret = NULL;
	int i;

	for (i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
		const struct test_case* tc = test_cases + i;
		if (tc->num == num && strcmp(tc->test_suite, test_suite) == 0) {
			ret = tc;
			break;
		}
	}
	return ret;
}

static void usage(void)
{
	int i;
	printf("Usage: %s -s test_suite -n test_number [-h] -- [args]\n", the_app);
	printf("   -s      test suite name.\n");
	for (i = 0; i < sizeof(test_suites)/sizeof(test_suites[0]); i++) {
		printf("             %s\n", test_suites[i].name);
	}
	printf("   -n      test case number.\n");
	printf("   -h      show this message.\n");
	printf("   args    test case arguments.\n");
}

int main(int argc, char** argv)
{
	const struct test_case* tc;
	const char* result;
	const char* ts = NULL;
	int num = INT_MIN;
	int opt;
	int i;

	the_app = argv[0];
	while ((opt = getopt(argc, argv, "s:n:h")) != -1) {
		switch (opt) {
		case 's':
			ts = optarg;
			break;
		case 'n':
			num = atoi(optarg);
			break;
		case 'h':
			usage();
			return EXIT_SUCCESS;
		default:
			usage();
			return EXIT_FAILURE;
		}
	}
	argv[optind - 1] = argv[0];
	argv += (optind - 1);
	argc -= (optind - 1);
	optind = 1;

	/* validate */
	if (ts == NULL || num == INT_MIN) {
		usage();
		return EXIT_FAILURE;
	}

	/* find */
	tc = find_test_case(ts, num);
	if (tc == NULL) {
		printf("%s#%d is not found.\n", ts, num);
		return EXIT_FAILURE;
	}

	/* print info */	
	printf("TEST_SUITE: %s\n", tc->test_suite);
	printf("TEST_NUMBER: %d\n", tc->num);
	printf("ARGS: ");
	for (i = 1; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");

	/* run */	
	fflush(stdout);
	result = run_test_case(tc, argc, argv);
	if (result) {
		printf("RESULT: %s\n", result);
		return EXIT_FAILURE;
	}
	printf("RESULT: ok\n");
	return EXIT_SUCCESS;
}


/**
 * テンプファイルを生成して、テンプファイルへのパスを返します。
 * ファイルの内容は終端が'Z'になること以外は不定となります。
 * 本関数が返したテンプファイルはtest_mck_delete_temp_fileで破棄する必要があります。 
 * @param prefix テンプファイルパスのプレフィックス
 * @param size ファイルサイズ（バイト単位）
 * @retval NULL 添付ファイルの作成に失敗
 * @return 非NULL 添付ファイルへのパス
 */
char* test_mck_create_temp_file(const char* prefix, size_t size)
{
	char* path = NULL;
	int fd = -1;
	int state;

	state = asprintf(&path, "%s-XXXXXX", prefix);
	if (state == -1) {
		goto fail;
	}

	fd = mkstemp(path);
	if (fd == -1) {
		goto fail;
	}

	state = lseek(fd, size-1, 0);
	if (state == -1) {
		goto fail;
	}
	write(fd, "Z", 1);
	fsync(fd);
	close(fd);
	return path;
fail:
	if (fd != -1) {
		close(fd);
		unlink(path);
	}
	if (path != NULL) {
		free(path);
	}
	return NULL;
}

/**
 * テンプファイルを削除します。
 * @param path test_mck_create_temp_fileが返したアドレスを指定します。
 *             本関数呼び出し後はpathが示すアドレスは無効になります。
 */
void test_mck_delete_temp_file(char* path)
{
	if (path == NULL) {
		return;
	}
	unlink(path);
	free(path);
}

/**
 * 文字列を unsigned long に変換します。
 * 変換が不可能な場合default_valueを返します。
 */
unsigned long test_mck_strtoul(const char *nptr, unsigned long default_value)
{
	unsigned long ret;
	char* endptr;

	if (nptr == NULL) {
		return default_value;
	}

	errno = 0;
	ret = strtoll(nptr, &endptr, 0);
	if (ret == 0 && endptr == nptr) {
		ret = default_value;
	}
	else if ((ret == LONG_MIN || ret == LONG_MAX)
		 && errno == ERANGE) {
		ret = default_value;
	}
	return ret;
}

/**
 * gettidのラッパー関数です。
 */
pid_t gettid(void)
{
	return (pid_t)syscall(__NR_gettid);
}

/**
 * 仮想アドレスに対応する物理メモリの情報を返します。
 */
#define PM_ENTRY_BYTES      sizeof(unsigned long)
#define PM_STATUS_BITS      3
#define PM_STATUS_OFFSET    (64 - PM_STATUS_BITS)
#define PM_STATUS_MASK      (((1LL << PM_STATUS_BITS) - 1) << PM_STATUS_OFFSET)
#define PM_STATUS(nr)       (((nr) << PM_STATUS_OFFSET) & PM_STATUS_MASK)
#define PM_PSHIFT_BITS      6
#define PM_PSHIFT_OFFSET    (PM_STATUS_OFFSET - PM_PSHIFT_BITS)
#define PM_PSHIFT_MASK      (((1LL << PM_PSHIFT_BITS) - 1) << PM_PSHIFT_OFFSET)
#define PM_PSHIFT(x)        (((uint64_t) (x) << PM_PSHIFT_OFFSET) & PM_PSHIFT_MASK)
#define PM_PFRAME_MASK      ((1LL << PM_PSHIFT_OFFSET) - 1)
#define PM_PFRAME(x)        ((x) & PM_PFRAME_MASK)
#define PM_PRESENT          PM_STATUS(4LL)
#define PM_SWAP             PM_STATUS(2LL)

static int __get_memory_info(const char* path, unsigned long virt, struct memory_info* info)
{
	int ret = 0;
	int fd = -1;
	unsigned long pagemap = 0;
	off_t offset = 0;

	if (info == NULL) {
		ret = -EINVAL;
		goto out;
	}
	memset(info, 0, sizeof(*info));

	/* open */
	if ((fd = open(path, O_RDONLY)) == -1) {
		printf("%s open() failed. %d\n", path, errno);
		ret = -EIO;
		goto out;
	}

	/* calc offset */
	offset = virt & PAGE_MASK;
	offset /= PAGE_SIZE;
	offset *= PM_ENTRY_BYTES;

	/* lseek */
	if ((lseek(fd, offset, SEEK_SET)) == -1) {
		printf("%s lseek() failed. %d\n", path, errno);
		ret = -EIO;
		goto out_close;
	}

	/* read */
	if ((read(fd, &pagemap, sizeof(pagemap))) == -1) {
		printf("%s offset:%lx read() failed. %d\n", path, offset, errno);
		ret = -EIO;
		goto out_close;
	}

	info->phys = ((pagemap & PM_PFRAME_MASK) << PAGE_SHIFT) | (virt & PAGE_OFFSET);
	info->pgsize = 1UL << ((pagemap & PM_PSHIFT_MASK) >> PM_PSHIFT_OFFSET);
	info->present = !!(pagemap & PM_PRESENT);
	info->swap = !!(pagemap & PM_SWAP);
out_close:
	if (fd != -1) {
		close(fd);
	}
out:
	return ret;
}

int get_memory_info_self(unsigned long virt, struct memory_info* info)
{
	const char* path = "/proc/self/pagemap";
	return __get_memory_info(path, virt, info);
}

int get_memory_info(pid_t pid, unsigned long virt, struct memory_info* info)
{
	char path[64];
	snprintf(path, sizeof(path), "/proc/%d/pagemap", pid);
	return __get_memory_info(path, virt, info);
}
