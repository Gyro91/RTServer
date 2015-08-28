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



/** creates a folder with name folder with set CPU for the cpuset */

void setup_affinity_folder(char *folder, char *cpu)
{
	FILE * f;
	char cpuset_folder[100];
	char cpuset_file[100];

	strcpy(cpuset_folder, "/sys/fs/cgroup/cpuset/");
	strcat(cpuset_folder, folder);

	// Creates the folder for the cpuset

	printf("#Creating folder \"%s\"\n", cpuset_folder);

	rmdir(cpuset_folder);
	if (mkdir(cpuset_folder, S_IRWXU))
		printf("Error creating CPUSET folder\n");

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

	// Sets which CPU will be used

	strcpy(cpuset_file, cpuset_folder);
	strcat(cpuset_file, "/cpuset.cpus");
	f = fopen(cpuset_file, "w");
	if (f == NULL) {
		printf("Error opening file \"%s\"\n", cpuset_file);
		exit(1);
	}
	fprintf(f, "%s", cpu);

	fclose(f);

}

/** sets a CPU in folder for the process  */

void set_affinity_cpu(char *folder, int pid)
{
	FILE * f;
	char cpuset_file[100];

	// Creates the folders for the cpuset

	strcpy(cpuset_file, "/sys/fs/cgroup/cpuset/");
	strcat(cpuset_file, folder);
	strcat(cpuset_file, "/tasks");
	f = fopen(cpuset_file, "w");
	if (f == NULL) {
		printf("Error opening file \"%s\"\n", cpuset_file);
		exit(1);
	}
	printf("#Setting affinity to %d\n", pid);
	fprintf(f, "%d\n", pid);
	fclose(f);

}


int main(int argc, char *argv[])
{
	// taking options : number of task
	//int ntask = 100;
	int pid_d, pid_c1, pid_c2, status;
	char buffer[10];

	// Creates consumers & dispatcher

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

				//setup_affinity_folder("dispatcher", "1");
				//set_affinity_cpu("dispatcher", pid_d);

				// Waits children

				wait(&status);
			}

		}

	}

	return 0;

}
