#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



int main(int argc, char *argv[])
{
	// taking options : number of task
	//int ntask = 100;
	int pid_d, pid_c1, status;
	char buffer[10];

	// creates consumers & dispatcher
	// father assigns cpu to dispatcher and consumers,then suspends
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
					buffer, (char*)NULL);
			printf("Exec fallita!\n");
			exit(1);
		}
		else
			wait(&status); // wait termination child
	}



	/* consumer1 creating n thread
	 * send a signal_ready to dispatcher
	 *
	 */

	// consumer2 like consumer1
	return 0;
}
