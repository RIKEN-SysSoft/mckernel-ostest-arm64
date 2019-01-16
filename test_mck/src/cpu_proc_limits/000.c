/* 000.c COPYRIGHT FUJITSU LIMITED 2015-2018 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "test_mck.h"
#include "testsuite.h"

#define PIPE_BUF_SIZE 0x10

#define PIDX_READ  0
#define PIDX_WRITE 1

#define CHILD_SLEEP_USEC	100000

typedef struct {
	int p2c[2];
	int c2p[2];
} proc_pipes_t;

static void child_fn(int id, int read_fd, int write_fd)
{
	char buf[PIPE_BUF_SIZE];

	/* wait until read success... */
	read(read_fd, buf, PIPE_BUF_SIZE);
	// fprintf(stdout, "[child]pid=%d, fork_id=%d, buf=%s\n", getpid(), id, buf);

	usleep(CHILD_SLEEP_USEC);
	memset(buf, 0, sizeof(buf));

	/* write back to parent. */
	snprintf(buf, sizeof(buf), "%010d", getpid());
	write(write_fd, buf, sizeof(buf));
}

void close_pipes(int *fd)
{
	close(fd[0]);
	close(fd[1]);
}

SETUP_FUNC(TEST_SUITE, TEST_NUMBER)
{
	static struct cpu_proc_limits_args args;
	int opt;

	memset(&args, 0 ,sizeof(args));

	while ((opt = getopt(tc_argc, tc_argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			args.proc_num = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return &args;
}

RUN_FUNC(TEST_SUITE, TEST_NUMBER)
{
	int i;
	int pid1;
	int forks = 0;
	char buf[PIPE_BUF_SIZE];
	struct cpu_proc_limits_args *args = (struct cpu_proc_limits_args*)tc_arg;
	proc_pipes_t *pp = NULL;

	tp_assert(args != NULL, "internal error.");
	tp_assert(0 < args->proc_num, "-p <child proc num> invalid argument.");

	pp = calloc(args->proc_num, sizeof(proc_pipes_t));
	tp_assert(pp != NULL, "calloc error.");

forkone:
	/* Create a pipe for synchronization */
	{
		int p2c_ret = 0, c2p_ret = 0;

		p2c_ret = pipe(pp[forks].p2c);
		c2p_ret = pipe(pp[forks].c2p);

		if(p2c_ret == -1 || c2p_ret == -1){
			for(i=0; i<forks; i++){
				close_pipes(pp[i].p2c);
				close_pipes(pp[i].c2p);
			}
			free(pp);
			tp_assert(NULL, "pipe open faild!!\n");
		}
	}

	/* Create child process. */
	pid1 = fork();

	if(pid1 != 0){
		/* parent */
		++forks;
		if ((pid1 != (-1)) && (forks < args->proc_num)) {
			goto forkone;
		}
		else if (pid1 < 0) {
			free(pp);
			tp_assert(NULL, "fork() Failed." );
		}
	} else {
		/* child */
		child_fn(forks, pp[forks].p2c[PIDX_READ], pp[forks].c2p[PIDX_WRITE]);

		close_pipes(pp[forks].p2c);
		close_pipes(pp[forks].c2p);

		free(pp);
		_exit(0);
	}

	// printf("Kick children process.\n");
	// printf("When fork_id is overlap, then error.\n");
	snprintf(buf, sizeof(buf), "%010d", getpid());
	for(i=0; i<args->proc_num; i++){
		write(pp[i].p2c[PIDX_WRITE], "1234567890", PIPE_BUF_SIZE+1);
	}

	printf("Wait children's process will be finished. (about %dmsec...)\n", CHILD_SLEEP_USEC / 1000);
	for(i=0; i<args->proc_num; i++){
		memset(buf, 0, sizeof(buf));

		read(pp[i].c2p[PIDX_READ], buf, PIPE_BUF_SIZE);
		// printf("[parent]child process(pid=%s) is finished.\n", buf);

		close_pipes(pp[i].p2c);
		close_pipes(pp[i].c2p);
	}
	free(pp);

	/* テスト成功 */
	return NULL;
}

TEARDOWN_EMPTY(TEST_SUITE, TEST_NUMBER)
