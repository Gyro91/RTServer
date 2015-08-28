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

pthread_mutex_t mux_np = PTHREAD_MUTEX_INITIALIZER; /** mutex for named pipe */
pthread_mutex_t mux_ft = PTHREAD_MUTEX_INITIALIZER; /** mutex for
														finishing_time */
pthread_mutex_t console_mux = PTHREAD_MUTEX_INITIALIZER;; /* mutex for console video */

char consumer_x[DIM_NAME];/** name of the consumer */
struct processing_task pt[500]; /** circular array for
								* recording processing times */
int next;/** index circular array */



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



/** sets cpu for the task caller */

void set_affinity()
{
	FILE * f;
	char cpuset_file[100];
	
	// Creates the folders for the cpuset
	
	if (!strcmp(consumer_x, "consumer1"))
		strcpy(cpuset_file, "/sys/fs/cgroup/cpuset/consumer_1");
	else
		strcpy(cpuset_file, "/sys/fs/cgroup/cpuset/consumer_2");
	
	strcat(cpuset_file, "/tasks");
	f = fopen(cpuset_file, "w");
	if (f == NULL) {
		printf("Error opening file \"%s\"\n", cpuset_file);
		exit(1);
	}
	printf("#Setting affinity to %ld\n", gettid());
	fprintf(f, "%ld\n", gettid());
	
	fclose(f);
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
	char *buffer;  /* buffer for data */
	message_t mess;
	
	pthread_mutex_lock(&console_mux);

	printf("%s Thread[%ld] setup\n", consumer_x, gettid());

	pthread_mutex_unlock(&console_mux);

	set_scheduler();
	set_affinity();

	while( LOOP ){

		/* Enter to critical section to protect named pipe */

		pthread_mutex_lock(&mux_np);

		/* Reading size data to be received */

		mkfifo(arg, 0666);
		fd = open(arg, O_RDONLY);
		read(fd, &mess, sizeof(message_t));



        #ifndef TEST
		printf("#Starting job %s\n", consumer_x);
        #endif

		/* Allocating memory for size byte */

		buffer = (char *)malloc(mess.size);

		/* Reading data */

		read(fd, buffer, mess.size);
		close(fd);
		unlink(arg);


		/* Exit critical section for named pipe*/

		pthread_mutex_unlock(&mux_np);

		/* Elaborating data */

		for(i = 0; i < 10000; i++)
			hash(buffer);

		/* Enter to critical section to protect struct for finishing_time */

		pthread_mutex_lock(&mux_ft);

		#ifndef TEST
		printf("#Finished job %s\n\n", consumer_x);
        #endif

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
			strcpy(path, "media/out_c1.txt");
		else
			strcpy(path, "media/out_c2.txt");

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
		//printf("\n");

		// Sleep to next period

		clock_nanosleep(CLOCK_MONOTONIC,
						TIMER_ABSTIME, &t, NULL);
		time_add_ms(&t, period);
	}

	pthread_exit(0);

}



/** returns 0 if it's consumer1 else 1 */

int who_am_i()
{
	if( strcmp(consumer_x, "consumer1") == 0 )
		return 0;
	else
		return 1;
}



/** changes output to new terminal */

void change_terminal()
{
	int fd;
	char *myfifo = "/tmp/myfifo3";
    char *myfifo_c = "/tmp/myfifo4";
	char path[14]; /* buffer /dev/pts/xxxx */
	char signal[6] = "start";


    printf("#Waiting for process output\n");

	/* reading path new terminal */
	mkfifo(myfifo, 0666);
    fd = open(myfifo, O_RDONLY);
    read(fd, path, 15);
    close(fd);
    unlink(myfifo);

    /* associating new output to the process */
    freopen(path, "a", stdout);

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

	while( LOOP ){
		mkfifo(myfifo, 0666);
		fd = open(myfifo, O_RDONLY);
		read(fd, buf, 6); /* reading path new terminal */
		close(fd);
		unlink(myfifo);

		if( strcmp(buf, "start") == 0 )
			break;
	}

}



/** It does preliminary operations for set_affinity */

void setup_affinity_folder(char * consumer_x)
{
	FILE *f;
	char cpuset_folder[100];
	char cpuset_file[100];
	
	// Creates the folders for the cpuset
	
	if (!strcmp(consumer_x, "consumer1"))
		strcpy(cpuset_folder, "/sys/fs/cgroup/cpuset/consumer_1");
	else
		strcpy(cpuset_folder, "/sys/fs/cgroup/cpuset/consumer_2");
	
	printf("#Creating folder \"%s\"\n", cpuset_folder);
	rmdir(cpuset_folder);
	if (mkdir(cpuset_folder, S_IRWXU)) {
		pthread_mutex_lock(&console_mux);	    
		printf("Error creating CPUSET folder\n");
		pthread_mutex_unlock(&console_mux);
		pthread_exit(NULL);
	}
	
	// Updates the memory node
	
	strcpy(cpuset_file, cpuset_folder);
	strcat(cpuset_file, "/cpuset.mems");
	f = fopen(cpuset_file, "w");
	if (f == NULL) {
		printf("Error opening file \"%s\"\n", cpuset_file);
		exit(1);
	}
	fprintf(f, "0");
	fclose(f);
	
	// Sets which CPU will be used by the consumers
	
	strcpy(cpuset_file, cpuset_folder);
	strcat(cpuset_file, "/cpuset.cpus");
	f = fopen(cpuset_file, "w");
	if (f == NULL) {
		printf("Error opening file \"%s\"\n", cpuset_file);
		exit(1);
	}
	if (!strcmp(consumer_x, "consumer1"))
		fprintf(f, "2");
	else
		fprintf(f, "3");
	
	fclose(f);
}



int main(int argc, char *argv[])
{
	int i, ntask, ret, status;
	pthread_t *t_list;
	
	/* to understand who's the process */
	strcpy(consumer_x, argv[0]);

	/* if consumer2 change output to new terminal */
	/*
	if( who_am_i() == 1 )
		change_terminal();
	else
		wait_consumer2();
	*/
	//printf("#Created %s\n", consumer_x);

	setup_affinity_folder(consumer_x);
	
	ntask = 3;//atoi(argv[1]); /* number of task to be created */
	next = 0;

	/* generating (ntask+1) thread */
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
