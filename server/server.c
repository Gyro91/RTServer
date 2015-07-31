#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
/** sets a cpu for each child */

void set_cpu(int pid1, int pid2 , int pid3)
{
	cpu_set_t bitmap;

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(1, &bitmap); // setting bitmap to 1

	sched_setaffinity(pid1, sizeof(bitmap), &bitmap); /* pid1 takes cpu1*/

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(2, &bitmap); // setting bitmap to 2

	sched_setaffinity(pid2, sizeof(bitmap), &bitmap); /* pid2 takes cpu2*/

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(3, &bitmap); // setting bitmap to 3

	sched_setaffinity(pid2, sizeof(bitmap), &bitmap); /* pid3 takes cpu3*/

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
		/* becomes dispatcher */
		execlp("./dispatcher",
				"dispatcher", (char*)NULL);
		printf("Exec fallita!\n");
		exit(1);
	}
	else{
		pid_c1 = fork();
		if( pid_c1 == 0 ){
			/* becomes consumer 1 */
			execlp("./consumer1", "consumer1",
					buffer, "/tmp/myfifo1", (char*)NULL);
			printf("Exec fallita!\n");
			exit(1);
		}
		else{
			pid_c2 = fork();
			if( pid_c2 == 0 ){
			/* becomes consumer 2 */
				execlp("./consumer2", "consumer2",
						buffer, "/tmp/myfifo2", (char*)NULL);
				printf("Exec fallita!\n");
				exit(1);
			}
			else{
				/* father assigns cpu to dispatcher
				* and consumers,then suspends
				*/
				//set_cpu(pid_d, pid_c1, pid_c2);
				wait(&status);
			}

		}

	}

	return 0;

}
