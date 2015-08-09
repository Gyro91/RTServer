#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "consumers.h"
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <pthread.h>
#define LOOP 1

pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mux_r = PTHREAD_MUTEX_INITIALIZER;
char consumer_i[10];

struct response_task rt[500];
int next;

//unsigned int C[]; /** It contains runtime for each task */
//unsigned int D[]; /** It contains deadline for each task(equal server deadline) */
//unsigned int T[]; /** It contains period for each server */
//task_attr *tk; // pointer to the array of task_attr

/** @brief generates a configuration for scheduling
 *
 *  generating a configuration for N tasks:
 *	- generates utilization factor for each task i Ui = 0.95/N
 *  - establishing server period and deadline
 *  - calculating budget for each task
 * */

void generate_conf()
{

}

/** Every task does several hash for each string received */

size_t hash(char const *input) {

	const int ret_size = 32;
	size_t ret = 0x555555;
	const int per_char = 7;

	while (*input) {
		ret ^= *input++;
		ret = ((ret << per_char) | (ret >> (ret_size - per_char)));
	}
	return ret;
}

/** this is the body of each thread:
 * - reads until there's a message( read is blocking )
 * - reads size of the message
 * - reads the message
 * - elaborating
 * - reads again for another message to elaborate and so on
 *
 *  @parameter arg is the name of the named pipe
 * */

void *thread_main(void *arg)
{
	int i;
	int fd; /* file descriptor */
	char *myfifo = (char *)arg; /* name of named pipe */
	unsigned char size; /* size data to read*/
	char *buffer;  /* buffer for data */
	char buff_size[4]; /* to receive size */
	struct timespec ta, tf;

	while( LOOP ){

		/* taking arrival time */
		clock_gettime(CLOCK_MONOTONIC, &ta);

		/* enter to critical section */
		pthread_mutex_lock(&mux);

		time_copy(&rt[next].arrival_time, &ta);
		/* reading size */
		mkfifo(myfifo, 0666);
		fd = open(myfifo, O_RDONLY);
		read(fd, buff_size, 4);
		printf("#Starting job %s\n", consumer_i);
		size = atoi(buff_size);

		/* allocating memory for size byte */
		buffer = (char *)malloc(size);

		/* reading data */
		read(fd, buffer, size);
		close(fd);
		unlink(myfifo);

		printf("%s\n", buffer);
		for(i = 0; i < 1000; i++)
			hash(buffer);

		printf("#Finished job %s\n\n", consumer_i);
		free(buffer);

		clock_gettime(CLOCK_MONOTONIC, &tf);
		time_copy(&rt[next].finishing_time, &tf);

		if(next == 499)
			next = 0;
		else
			next++;

		rt[next].tid = getpid();

		pthread_mutex_unlock(&mux);

	}

	pthread_exit(0);
}

/** sets CPU-2 to thread with tid thread if consumer is 1
 * 	otherwise sets CPU-3 to consumer2
 */

void set_cpu_thread(pthread_t thread,char *consumer)
{
	cpu_set_t bitmap;

	CPU_ZERO(&bitmap);
	if(!strcmp(consumer_i,"consumer1"))
		CPU_SET(2, &bitmap);
	else
		CPU_SET(3, &bitmap);

	pthread_setaffinity_np(thread,sizeof(bitmap), &bitmap);

}


void *dummy_main(void *arg)
{
	int i;
	int period = 30000; /** dispatcher is a process periodic */
	struct timespec t;

	//long int response_time;

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, period);
	while( LOOP){
		printf("#Report Dummy task\n");

		for(i = 0; i < 500; i++){
			if(rt[i].tid != 0){
				printf("Response Thread %d %s\n",rt[i].tid, consumer_i);
				rt[i].tid = 0;
			}
		}

		// sleep to next period
		clock_nanosleep(CLOCK_MONOTONIC,
						TIMER_ABSTIME, &t, NULL);
		time_add_ms(&t, period);
	}

	pthread_exit(0);

}

int main(int argc, char *argv[])
{
	int i, ntask, ret, status;
	pthread_t *t_list;

	strcpy(consumer_i, argv[0]);
	printf("#Created %s\n", consumer_i);

	ntask = atoi(argv[1]); /* number of task to be created */
	next = 0;

	// generating (ntask+1) thread
	t_list = (pthread_t*)malloc(ntask * sizeof(pthread_t));
	ret = pthread_create(&t_list[0],
				         NULL, dummy_main, NULL);
	if( ret ){
				printf("Error pthread_create() dummy\n");
				exit(1);
			}

	for(i = 1; i < (ntask + 1); i++){
		ret = pthread_create(&t_list[i],
				             NULL, thread_main, (void *)argv[2]);
		if( ret ){
			printf("Error pthread_create()\n");
			exit(1);
		}

		set_cpu_thread(t_list[i], consumer_i);
	}

	/* process father waits the end of threads */
	for(i = 0; i < (ntask + 1); i++){
		ret = pthread_join(t_list[i],(void **)&status);
		if( ret > 0 ){
			printf("Error pthread_join()\n");
			exit(1);
		}
	}

	return 0;
}
