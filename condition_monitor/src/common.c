/* common.c COPYRIGHT FUJITSU LIMITED 2018 */
#include "common.h"
#include <errno.h>
#include <sys/epoll.h>

void print_test_overview(int tp_num)
{
	printf("# %2d : %s\n", tp_num, usage_messages[tp_num]);
}

int get_status(int os_num)
{
	int status = ihk_os_get_status(os_num);
	if (status < 0) {
		printf("ihk_os_get_status(): error(%d)\n", status);
	}
	return status;
}

int event_polling(int os_num, int type)
{
	int evfd = -1;
	int epfd = -1;
	int func_ret = -1;
	int lib_ret = -1;
	struct epoll_event event;
	struct epoll_event events[1];

	evfd = ihk_os_get_eventfd(os_num, type);
	if (evfd < 0) {
		printf("ihk_os_get_eventfd(): error(%d)\n", evfd);
		func_ret = evfd;
		goto out;
	}

	/* 
	 * the size argument(arg0) is ignored,
	 * but must be greater than zero (from manpage)
	 */
	epfd = epoll_create(1);
	if (epfd == -1) {
		perror("epoll_create()");
		func_ret = errno;
		goto out;
	}

	memset(&event, 0, sizeof(struct epoll_event));
	event.events = EPOLLIN;
	event.data.fd = evfd;

	lib_ret = epoll_ctl(epfd, EPOLL_CTL_ADD, evfd, &event);
	if (lib_ret != 0) {
		perror("epoll_ctl()");
		func_ret = errno;
		goto out;
	}

	do {
		int nfd = epoll_wait(epfd, events, 1, -1);
		int i;

		for (i = 0; i < nfd; i++) {
			if (events[i].data.fd == evfd) {
				printf("eventfd(os=%d, type=%d) epoll() wakeuped.\n",
					os_num, type);
				goto test_ok;
			}
		}
	} while(1);

test_ok:
	/* success */
	func_ret = 0;
out:
	return func_ret;
}
