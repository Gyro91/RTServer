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

pthread_mutex_t mux_np = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mux_ft = PTHREAD_MUTEX_INITIALIZER;
char consumer_i[10];/** name of the consumer */

struct response_task rt[500]; /** circular array for recording response times */
int next;/** index circular array */

pthread_barrier_t barr;

/** @brief calculates response time in ms */

long int calculate_rt(struct timespec *ta, struct timespec *tf)
{
	long int at, ft;

	at = (ta->tv_sec * 1000) + (ta->tv_nsec / 1000000);
	ft = (tf->tv_sec * 1000) + (tf->tv_nsec / 1000000);

	return (ft - at);
}

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
	int i, fd; /* count variable and descriptor */
	char *myfifo = (char *)arg; /* name of named pipe */
	char *buffer;  /* buffer for data */
	struct timespec tf;/* arrival and finishing time struct */
	message_t mess;

	while( LOOP ){

		/* enter to critical section to protect named pipe */
		pthread_mutex_lock(&mux_np);

		/* reading size data to be received */
		mkfifo(myfifo, 0666);
		fd = open(myfifo, O_RDONLY);
		read(fd, &mess, sizeof(message_t));

		/* copying arrival tyme in rt */
		time_copy(&rt[next].arrival_time, &mess.arrival_time);

		printf("#Starting job %s\n", consumer_i);
	//	printf("%ld.%ld\n",mess.arrival_time.tv_sec,mess.arrival_time.tv_nsec);
		/* allocating memory for size byte */
		buffer = (char *)malloc(mess.size);

		/* reading data */
		read(fd, buffer, mess.size);
		close(fd);
		unlink(myfifo);

		printf("%s\n", buffer);

		pthread_mutex_unlock(&mux_np);

		/* elaborating data */
		for(i = 0; i < 10000; i++)
			hash(buffer);

		/* enter to critical section to protect struct for finishing_time */
		pthread_mutex_lock(&mux_ft);

		printf("#Finished job %s\n\n", consumer_i);
		free(buffer);

		clock_gettime(CLOCK_MONOTONIC, &tf);
		time_copy(&rt[next].finishing_time, &tf);

		rt[next].tid = gettid();

		if(next == 499)
			next = 0;
		else
			next++;

		pthread_mutex_unlock(&mux_ft);

	//	pthread_barrier_wait(&barr);
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

/** body of dummy task.It's the task for calculating response time */

void *dummy_main(void *arg)
{
	int i;
	int period = 30000; /* dummy is a task periodic */
	struct timespec t;
	long int response_t;  /* response time */

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, period);
	while( LOOP){

		printf("#Report Dummy task %s\n", consumer_i);

		for(i = 0; i < 500; i++){
			if( rt[i].tid > 0 ){
				response_t = calculate_rt(&rt[i].arrival_time,
										  &rt[i].finishing_time);
				printf("Response time Thread[%d]: ", rt[i].tid);
				printf("%ld ms\n", response_t);
				rt[i].tid = 0;
			}
		}
		printf("\n");


		// sleep to next period
		clock_nanosleep(CLOCK_MONOTONIC,
						TIMER_ABSTIME, &t, NULL);
		time_add_ms(&t, period);
	}

	pthread_exit(0);

}

/** returns 0 if it's consumer1 else 1 */

int who_am_i()
{
	if( strcmp(consumer_i, "consumer1") == 0 )
		return 0;
	else
		return 1;
}

/** changes output to new terminal */

void change_terminal()
{
	int fd;
	char *myfifo = "/tmp/myfifo3"; /* my_fifo */
	char buf[15]; /* buffer */
	char signal[6] = "start";
    char *myfifo_c = "/tmp/myfifo4";

    printf("#Waiting for process output\n");

	/* reading path new terminal */
	mkfifo(myfifo, 0666);
    fd = open(myfifo, O_RDONLY);
    read(fd, buf, 15);
    close(fd);
    unlink(myfifo);

    /* associating new output to the process */
    freopen(buf, "a", stdout);

    /* sending signal to consumer1*/
    fd = open(myfifo_c, O_WRONLY);
    write(fd, signal, 6);
    close(fd);
}

/** consumer1 waits consumer2 to change terminal */

void wait_consumer2()
{
	int fd;
	char *myfifo = "/tmp/myfifo4"; /* my_fifo */
	char buf[15]; /* buffer */

	while( 1 ){
		mkfifo(myfifo, 0666);
		fd = open(myfifo, O_RDONLY);
		read(fd, buf, 6); /* reading path new terminal */
		close(fd);
		unlink(myfifo);

		if( strcmp(buf, "start") == 0 )
			break;
	}

}


int main(int argc, char *argv[])
{
	int i, ntask, ret, status;
	pthread_t *t_list;

	/* to understand who's the process */
	strcpy(consumer_i, argv[0]);

	/* if consumer2 change output to new terminal */
	if( who_am_i() == 1 )
		change_terminal();
	else
		wait_consumer2();

	printf("#Created %s\n", consumer_i);

	ntask = 10;//atoi(argv[1]); /* number of task to be created */
	next = 0;
	pthread_barrier_init(&barr, NULL, 10);

	/* generating (ntask+1) thread */
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
		/* allocating cpu-unit for each thread */
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
