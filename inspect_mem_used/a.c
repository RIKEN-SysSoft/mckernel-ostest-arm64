/* a.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define KiB(n) ((n)*1024)
#define MiB(n) (KiB(n)*1024)
#define GiB(n) (GiB(n)*1024)

#define tp_assert(expr, str)				\
	do {						\
		if (!(expr)) {				\
			fprintf(stderr, "%s\n", (str));	\
			exit(-1);			\
		}					\
	} while(0)

//
// parameter
//
#define MAX_THREAD  48 //(52-4-1)

unsigned long nr_thread = 2;
unsigned long alloc_size = MiB(1);

void parse_args(int tc_argc, char** tc_argv)
{
	char* pch;
	int opt;
	while ((opt = getopt(tc_argc, tc_argv, "t:s:")) != -1) {
		switch (opt) {
		case 't':  // thread
			nr_thread = strtoul(optarg, &pch, 0);
			break;
		case 's':  // allocate size of bytes
			alloc_size = strtoul(optarg, &pch, 0);
			break;
		}
	}
	tp_assert(0 < nr_thread && nr_thread <= MAX_THREAD, "invalid argument(-t)\n");
	tp_assert(0 < alloc_size, "invalid argument(-s)\n");
}

volatile int wait = 1;
static void sig_handler(int signame)
{
	while(wait);
}


//
//
//
struct thread_data {
	unsigned long  num;
	unsigned long alloc_count;
	pthread_t tid;
	sem_t sync;
	sem_t start;
};


static void do_allocate(struct thread_data* data)
{
	while(1) {
		char* a = malloc(alloc_size);
		data->alloc_count++;
		//printf("[%lu] %lu\n", data->num, data->alloc_count);
		//fflush(stdout);
		memset(a, 0, alloc_size);
	}
}


static void* func(void *value)
{
	struct thread_data* data = (struct thread_data*)value;
	
	sem_post(&data->sync);
	//printf("[%lu] wait\n", data->num);

	sem_wait(&data->start);
	//printf("[%lu] start\n", data->num);

	do_allocate(data);
	return NULL;
}


static struct thread_data thread_data[MAX_THREAD] = {{0}};

int main(int argc, char** argv)
{
	int i;
	
	parse_args(argc, argv);

	// setup signal handler
	if (signal(SIGSEGV, sig_handler) == SIG_ERR) {
		tp_assert(NULL, "setup signal handler error.");
	}

	/* create */
	//parent
	memset(thread_data, 0, sizeof(struct thread_data));
	//children
	for (i = 1; i < nr_thread; i++) {
		thread_data[i].num = i;
		sem_init(&thread_data[i].sync, 0, 0);
		sem_init(&thread_data[i].start, 0, 0);
		pthread_create(&thread_data[i].tid, NULL, func, &thread_data[i]);
	}
	
	/* synchronization */
	for (i = 1; i < nr_thread; i++) {
		sem_wait(&thread_data[i].sync);
	}
	
	/* start */
	for (i = 1; i < nr_thread; i++) {
		sem_post(&thread_data[i].start);
	}
	do_allocate(&thread_data[0]);
	tp_assert(NULL, "error.");

	/* destroy */
	//for (i = 1; i < nr_thread; i++) {
	//	pthread_join(thread_data[i].tid, NULL);
	//	sem_destroy(&thread_data[i].sync);
	//	sem_destroy(&thread_data[i].start);
	//}
	return 0;
}
