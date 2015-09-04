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
#define TEST
#define LOOP 1
#define DIM_NAME 10
#define __sched_priority sched_priority

char *wc = "/tmp/wait_c";

/** mutex for named pipe */
pthread_mutex_t mux_np = PTHREAD_MUTEX_INITIALIZER;
/** mutex for finishing_time */
pthread_mutex_t mux_ft = PTHREAD_MUTEX_INITIALIZER;
/** mutex for console video */
pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;


/** name of the consumer */
char consumer_x[DIM_NAME];
/** circular array for recording processing times */
struct processing_task pt[500];
/** index circular array */
int next;



/** changes scheduler */

void set_scheduler()
{
	int ret;
	struct sched_attr attr;

	attr.size = sizeof(attr);
	attr.sched_flags = 0;
	attr.sched_nice = 0;
	attr.sched_priority = 0;

	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = 15 * 1000 * 1000;
	attr.sched_period = attr.sched_deadline = 50 * 1000 * 1000;
	
	ret = sched_setattr(0, &attr, 0);
	if (ret < 0) {
		pthread_mutex_lock(&console_mux);
	    perror("ERROR: sched_setattr");
	    printf("runtime: %lld\nperiod: %lld\ndeadline: %lld\n",
	           attr.sched_runtime,
	           attr.sched_period,
	           attr.sched_deadline);
	    pthread_mutex_unlock(&console_mux);
	    pthread_exit(NULL);
	  }

	printf("Set SCHED_DEADLINE %lld/%lld\n",
			attr.sched_runtime/ 1000000,
			attr.sched_deadline / 1000000);
}



/** Sets cpu for the tasks */

void set_cpu_thread(pthread_t thread)
{
	cpu_set_t bitmap;
	
	CPU_ZERO(&bitmap);
	
	if(!strncmp(consumer_x, "consumer1", DIM_NAME))
		CPU_SET(2, &bitmap);
	else
		CPU_SET(3, &bitmap);
	
	pthread_setaffinity_np(thread,sizeof(bitmap), &bitmap);

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
	int i, fd, ret, remaining; /* count variable and descriptor */
	char *buffer;  /* buffer for data */
	message_t mess;
	

//	set_scheduler();

	while( LOOP ){

		/* Enter to critical section to protect named pipe */

		pthread_mutex_lock(&mux_np);

		/* Reading size data to be received */

		mkfifo(arg, 0666);
		fd = open(arg, O_RDONLY);
		if( fd == -1 )
			printf("Error opening consumer\n");

		remaining = sizeof(message_t);
		while(remaining > 0){
			ret = read(fd, (void *)&mess + sizeof(message_t) - remaining, remaining);
			remaining -= ret;
			if(ret == -1){
				printf("Error read1\n");
			}
		}



		printf("#Starting job %s\n", consumer_x);


		/* Allocating memory for size byte */

		buffer = (char *)malloc(mess.size);

		/* Reading data */
		remaining = mess.size;
		while(remaining > 0){
			ret = read(fd, buffer + mess.size - remaining, remaining);
			remaining -= ret;
			if( ret == -1){
				printf("Error read2\n");
			}
		}

		close(fd);
		unlink(arg);


		/* Exit critical section for named pipe*/

		pthread_mutex_unlock(&mux_np);

		/* Elaborating data */

		for(i = 0; i < 100000; i++)
			hash(buffer);

		/* Enter to critical section to protect struct for finishing_time */

		pthread_mutex_lock(&mux_ft);

		printf("#Finished job %s\n\n", consumer_x);

		/* Sending signal to dispatcher to close */

		fd = open(wc, O_WRONLY);

		write(fd, "Hi", sizeof("Hi"));

		close(fd);

		/* Saving finishing & arrival, tid
		 * and update index array */

		time_copy(&pt[next].arrival_time, &mess.arrival_time);
		clock_gettime(CLOCK_MONOTONIC, &pt[next].finishing_time);

		pt[next].tid = gettid();
		if(next == 499)
			next = 0;
		else
			next++;

		/* exit critical section for finishing_time */

		pthread_mutex_unlock(&mux_ft);

		free(buffer);
	}

	pthread_exit(0);
}



/** body of dummy task.It's the task for calculating response time */

void *dummy_main(void *arg)
{
	int i;
	int period = 5000; /* dummy is a task periodic */
	struct timespec t;
	long int processing_t;  /* processing time */

	#ifdef TEST
		FILE *f;
		char path[17];

		if(strcmp(consumer_x, "consumer1") == 0)
			strcpy(path, "../script/tools/out_c1.txt");
		else
			strcpy(path, "../script/tools/out_c2.txt");

	#endif

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_add_ms(&t, period);
	while( LOOP){

		printf("#Report Dummy task %s\n", consumer_x);

		pthread_mutex_lock(&mux_ft);
		for(i = 0; i <= next; i++){
			if( pt[i].tid > 0 ){
				processing_t = calculate_pt(&pt[i].arrival_time,
											&pt[i].finishing_time);

                #ifndef TEST

				printf("Processing time Thread[%ld]: ", pt[i].tid);
				printf("%ld ms\n", processing_t);

				#endif

				#ifdef TEST

				f = fopen(path, "a");
				if(f == NULL){
					perror("Impossibile aprire il file\n");
					exit(1);
				}

				fprintf(f, "%ld\n", processing_t);

				fclose(f);

				#endif

				pt[i].tid = 0;

			}
		}
		next = 0;
		pthread_mutex_unlock(&mux_ft);


		// Sleep to next period

		clock_nanosleep(CLOCK_MONOTONIC,
						TIMER_ABSTIME, &t, NULL);
		time_add_ms(&t, period);
	}

	pthread_exit(0);

}







int main(int argc, char *argv[])
{
	int i, ntask = 3, ret, status;
	pthread_t *t_list;
	
	/* to understand who's the process */

	strcpy(consumer_x, argv[0]);

	printf("#Created %s\n", consumer_x);

	next = 0;

	/* Generating (ntask+1) thread */

	t_list = (pthread_t*)malloc((ntask+1) * sizeof(pthread_t));
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

		/* Allocating cpu-unit for each thread */

		set_cpu_thread(t_list[i]);
	}

	/* Father waits the end of threads */

	for(i = 0; i < (ntask + 1); i++){
		ret = pthread_join(t_list[i],(void **)&status);
		if( ret > 0 ){
			printf("Error pthread_join()\n");
			exit(1);
		}
	}

	return 0;
}
