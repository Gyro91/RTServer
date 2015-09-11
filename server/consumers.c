#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "consumers.h"
#include "mqueue.h"

#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <pthread.h>
#define TEST
#define LOOP 1
#define DIM_NAME 10
#define __sched_priority sched_priority
#define DIM 10000



/** mutex for named pipe */
pthread_mutex_t mux_np = PTHREAD_MUTEX_INITIALIZER;
/** mutex for finishing_time */
pthread_mutex_t mux_ft = PTHREAD_MUTEX_INITIALIZER;
/** mutex for console video */
pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;


/** name of the consumer */
char consumer_x[DIM_NAME];
/** circular array for recording processing times */
struct processing_task pt[DIM];
/** index circular array */
int next;
/** Descriptor for queue*/
mqd_t mq;

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
	attr.sched_runtime = 5 * 1000 * 1000;
	attr.sched_period = attr.sched_deadline = (attr.sched_runtime / 0.95);
	
	ret = sched_setattr(0, &attr, 0);
	if (ret < 0) {
	    pthread_mutex_lock(&console_mux);
	    perror("ERROR: sched_setattr");
	    pthread_mutex_unlock(&console_mux);
	    pthread_exit(NULL);
	  }

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

/** Open queues for communication with Dispatcher */

void open_queues()
{
	struct mq_attr attr;

	 if(strcmp(consumer_x, "consumer1") == 0)
		 unlink(QUEUE_NAME1);
	 else
		 unlink(QUEUE_NAME2);

	/* Initialize the queue attributes */

	attr.mq_flags = 0;
    attr.mq_maxmsg = 5000;
    attr.mq_msgsize = DIM_MAX_PAYLOAD;
    attr.mq_curmsgs = 0;

    /* Creates the message queue */

    if(strcmp(consumer_x, "consumer1") == 0)
        mq = mq_open(QUEUE_NAME1, O_CREAT | O_RDONLY, 0644, &attr);
    else
    	mq = mq_open(QUEUE_NAME2, O_CREAT | O_RDONLY, 0644, &attr);

    CHECK((mqd_t)-1 != mq);



}

/** this is the body of each thread:
 * - reads until there's a message( read is blocking )
 * - reads size of the messagefd_w = open(wc, O_WRONLY);
		ret = write(fd_w, &c, 1);
		if( ret == -1 )
			perror("Error write wait_c\n");

		close(fd_w);
 * - reads the message
 * - elaborating
 * - reads again for another message to elaborate and so on
 *
 *  @parameter arg is the name of the named pipe
 * */

void *thread_main(void *arg)
{
	int i;
	size_t  bytes_read;
	char buffer[DIM_MAX_PAYLOAD];  /* buffer for data */
	message_t mess;
	
	open_queues();
	set_scheduler();

	while( LOOP ){

		/* Enter to critical section to protect named pipe */

		pthread_mutex_lock(&mux_np);

		/* Reading size data to be received */

		bytes_read = mq_receive(mq,(char *)&mess, 56, NULL);
		CHECK(bytes_read >= 0);

		/* Allocating memory for size byte */



		/* Reading data */
		bytes_read = mq_receive(mq, buffer, DIM_MAX_PAYLOAD, NULL);
		CHECK(bytes_read >= 0);

		//printf("%s\n", buffer);

		pthread_mutex_unlock(&mux_np);


		/* Elaborating data */

		for(i = 0; i < 10000; i++)
			hash(buffer);

		/* Enter to critical section to protect struct for finishing_time */

		pthread_mutex_lock(&mux_ft);

		/* Saving finishing & arrival, tid
		 * and update index array */

		time_copy(&pt[next].arrival_time, &mess.arrival_time);
		clock_gettime(CLOCK_MONOTONIC, &pt[next].finishing_time);

		pt[next].tid = gettid();
		next = (next + 1) % DIM;

		/* Exit critical section for finishing_time */

		pthread_mutex_unlock(&mux_ft);

	//	free(buffer);
	}

	pthread_exit(0);
}



/** body of dummy task.It's the task for calculating response time */

void *dummy_main(void *arg)
{
	int i, count = 0;
	int period = 1000; /* dummy is a task periodic */
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

		printf("#Report Dummy task %s : packet:%d\n", consumer_x, count);

		pthread_mutex_lock(&mux_ft);
		for(i = 0; i < next; i++){
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
				count++;
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

	//	set_cpu_thread(t_list[i]);
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
