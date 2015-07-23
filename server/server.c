#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "consumers.h"
#include <sys/types.h>
#include <sys/wait.h>
//task_attr *tk; // pointer to the array of task_attr

//unsigned int C[]; /** It contains runtime for each task */
//unsigned int D[]; /** It contains deadline for each task(equal server deadline) */
//unsigned int T[]; /** It contains period for each server */

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

int main(int argc, char *argv[])
{
	// taking options : number of task
	//int ntask = 100;
	int pid_d, status;
	// generates configurations for consumers threads

	// creates consumers & dispatcher
	// father assigns cpu to dispatcher and consumers,then suspends

	pid_d = fork();
	if( pid_d == 0 ){
		// becomes dispatcher
		execlp("./dispatcher", "dispatcher", (char*)NULL);
		printf("Exec fallita!\n");
		exit(1);
	}
	else{

		wait(&status);

	}



	/* consumer1 creating n thread
	 * send a signal_ready to dispatcher
	 *
	 */

	// consumer2 like consumer1
	return 0;
}
