#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <sched.h>


/** Sets affinity CPU-1 to Dispatcher */

void set_affinity(int pid_d)
{
	cpu_set_t bitmap;

	CPU_ZERO(&bitmap);
	CPU_SET(1, &bitmap);

	sched_setaffinity(pid_d, sizeof(bitmap), &bitmap);
}


int main(int argc, char *argv[])
{
	// taking options : number of task
	//int ntask = 100;
	int pid_d, pid_c1, pid_c2, status;
	char buffer[10];

	// creates consumers & dispatcher

	snprintf(buffer, 10, "%d", 100);
	pid_d = fork();
	if( pid_d == 0 ){

		// Becomes dispatcher

		execlp("./dispatcher",
				"dispatcher", (char*)NULL);
		printf("Exec fallita!\n");
		exit(1);
	}
	else{
		pid_c1 = fork();
		if( pid_c1 == 0 ){

			// Becomes consumer 1

			execlp("./consumer1", "consumer1",
					buffer, "/tmp/myfifo1", (char*)NULL);
			printf("Exec fallita!\n");
			exit(1);
		}
		else{
			pid_c2 = fork();
			if( pid_c2 == 0 ){

				// Becomes consumer2

				execlp("./consumer2", "consumer2",
						buffer, "/tmp/myfifo2", (char*)NULL);
				printf("Exec fallita!\n");
				exit(1);
			}
			else{

				// Setting affinity to dispatcher

				set_affinity(pid_d);

				// Waits children

				wait(&status);
			}

		}

	}

	return 0;

}
