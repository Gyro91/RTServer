#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "consumers.h"
#define LOOP 1

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

/** this is the body of each thread*/

void *thread_main(void *arg)
{

	printf("%d\n",*(int *)arg);

	pthread_exit(0);
}


int main(int argc, char *argv[])
{
	int i, ntask, ret, status;
	pthread_t * t_list;

	printf("#Created consumer\n");

	ntask = atoi(argv[1]); /* number of task to be created */
	// generates configurations for consumers threads
	// generating ntask thread
	t_list = (pthread_t*)malloc(ntask * sizeof(pthread_t));
	for(i = 0; i < ntask; i++){
		ret = pthread_create(&t_list[i],
							 NULL, thread_main, (void *)&i);
		if( ret ){
			printf("Error pthread_create()\n");
			exit(1);
		}
	}

	/* process father waits the end of threads */
	for(i = 0; i < ntask; i++){
			ret = pthread_join(t_list[i],(void **)&status);
			if( ret ){
				printf("Error pthread_wait()\n");
				exit(1);
			}
		}
	printf("%d\n",ntask);


	return 0;
}
