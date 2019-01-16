/* freeze_thaw.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ihklib.h>

static void usage(char *cmd)
{
	printf("Usage:\n");
	printf("  %s <os_number> [freeze | thaw]\n", cmd);
}

static int do_freeze(int os_num)
{
	unsigned long os_set = (1UL << os_num);
	return ihk_os_freeze(&os_set, 1);
}

static int do_thaw(int os_num)
{
	unsigned long os_set = (1UL << os_num);
	return ihk_os_thaw(&os_set, 1);
}

int main(int argc, char *argv[])
{
	int os_num = 0;
	int ret = -1;

	if (argc < 3) {
		usage(argv[0]);
		goto out;
	}

	os_num = atoi(argv[1]);
	if (os_num < 0 || 63 < os_num) {
		printf("Invalid argument 1 os_num.\n");
		usage(argv[0]);
		goto out;
	}

	/* freeze */
	if (strcmp(argv[2], "freeze") == 0) {
		ret = do_freeze(os_num);
		if (ret != 0) {
			printf("ihk_freeze failed.\n");
		}
		goto out;
	}

	/* thaw */
	if (strcmp(argv[2], "thaw") == 0) {
		ret = do_thaw(os_num);
		if (ret != 0) {
			printf("ihk_thaw failed.\n");
		}
		goto out;
	}
	printf("Invalid argument 2 string.\n");
	usage(argv[0]);

out:
	return ret;
}
