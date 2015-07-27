#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "consumers.h"
#define LOOP 1
#define MAX_BUF 1024


pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;

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

/** this is the body of each thread:
 * - reads until there's a message( read is blocking )
 * - reads size of the message
 * - reads the message
 * - elaborating
 * - reads again for another message to elaborate and so on
 * */

void *thread_main(void *arg)
{
	int fd; /* file descriptor */
	char *myfifo = (char *)arg;
	unsigned char size;
	char *buffer;
	char buff_size[4]; /* to receive size */

	while( LOOP ){
		pthread_mutex_lock(&mux);

		/* reading size */
		mkfifo(myfifo, 0666);
		fd = open(myfifo, O_RDONLY);
		read(fd, buff_size, 4);
		size = atoi(buff_size);

		/* allocating memory for size byte */
		buffer = (char *)malloc(size);

		/* reading data */
		read(fd, buffer, size);
		close(fd);
		unlink(myfifo);

		printf("%s\n", buffer);
		printf("%d\n", size);

		pthread_mutex_unlock(&mux);

		free(buffer);

	}

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
							 NULL, thread_main, (void *)argv[2]);
		if( ret ){
			printf("Error pthread_create()\n");
			exit(1);
		}
	}

	/* process father waits the end of threads */
	for(i = 0; i < ntask; i++){
			ret = pthread_join(t_list[i],(void **)&status);
			if( ret > 0 ){
				printf("Error pthread_join()\n");
				exit(1);
			}
		}
	printf("%d\n",ntask);


	return 0;
}
