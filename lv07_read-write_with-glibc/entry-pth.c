/* entry-pth.c COPYRIGHT FUJITSU LIMITED 2015-2017 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREAD_NUM 64

int thread_num;
char *filepath;
pthread_barrier_t barrier;

void read_write(int num, const char *path) {
	FILE* fp;
	char buf[256];
	size_t cnt;
	int i;

	if ((fp=fopen(path,"r")) == NULL) {
		perror("fopen failed: ");
		return;
	}

	buf[0] = '0' + num;	/* th[0]  = '0', th[1]  = '1', th[2]  = '2', th[3]  = '3', th[4]  = '4',
				   th[5]  = '5', th[6]  = '6', th[7]  = '7', th[8]  = '8', th[9]  = '9',
				   th[10] = ':', th[11] = ';', th[12] = '<', th[13] = '=', th[14] = '>',
				   th[15] = '?', th[16] = '@', th[17] = 'A', th[18] = 'B', th[19] = 'C',
				   th[20] = 'D', th[21] = 'E', th[22] = 'F', th[23] = 'G', th[24] = 'H',
				   th[25] = 'I', th[26] = 'J', th[27] = 'K', th[28] = 'L', th[29] = 'M',
				   th[30] = 'N', th[31] = 'O', th[32] = 'P', th[33] = 'Q', th[34] = 'R',
				   th[35] = 'S', th[36] = 'T', th[37] = 'U', th[38] = 'V', th[39] = 'W',
				   th[40] = 'X', th[41] = 'Y', th[42] = 'Z', th[43] = '[', th[44] = '\',
				   th[45] = ']', th[46] = '^', th[47] = '_', th[48] = '`', th[49] = 'a',
				   th[50] = 'b', th[51] = 'c', th[52] = 'd', th[53] = 'e', th[54] = 'f',
				   th[55] = 'g', th[56] = 'h', th[57] = 'i', th[58] = 'j', th[59] = 'k',
				   th[60] = 'l', th[61] = 'm', th[62] = 'n', th[63] = 'o' */

	buf[1] = ':';
	cnt = fread(buf+2, sizeof(buf[0]), sizeof(buf)/sizeof(buf[0])-2, fp);
	buf[cnt+2] = '\0';

	pthread_barrier_wait(&barrier);
	printf("%d: %s", num, buf);
	pthread_barrier_wait(&barrier);

	fclose(fp);
}

// The child thread will execute this function 
void *threadFunction( void* argument ) {
	int *num = (int *)argument;
	read_write(*num, filepath);
	return 0; 
}

int main(int argc, char** argv) {
	pthread_t child[MAX_THREAD_NUM];
	int i = 0, ret = 0;
	int tn[MAX_THREAD_NUM];
	int ncpu;

	if (argc != 3) {
		printf("Invalid number of arguments. Usage: %s <ostyp_-file_path> <thread_num>\n", argv[0]);
		return 0;
	}

	filepath = argv[1];

	thread_num = atoi(argv[2]);

	if ((thread_num < 1) || (MAX_THREAD_NUM < thread_num)) {
		printf("thread_number invalid, 1 <= thread_num <= %d\n", MAX_THREAD_NUM);
		return 0;
	}
	printf("use thread_num = %d.\n", thread_num);

	for (i = 0; i < thread_num; i++) {
		tn[i] = i;
	}

	ret = pthread_barrier_init(&barrier, NULL, thread_num);
	if (ret != 0) {
		printf("pthread_barrier_init error\n");
		return 0;
	}

	for(i = 1; i < thread_num; i++) {
		// Call the clone system call to create the child thread 
		ret = pthread_create(&child[i], NULL, threadFunction, (void *)&tn[i]);
		if(ret) {
			printf("Failed to create thread[%d]. ret = %d\n", i, ret);
			return 0;
		}
	}

	read_write(0, filepath);

	for(i = 1; i < thread_num; i++) {
		ret = pthread_join(child[i], NULL);
		if(ret) {
			printf("Failed to join thread[%d]. ret = %d\n", i, ret);
			return 0;
		}
	}

	return 0; 
}
