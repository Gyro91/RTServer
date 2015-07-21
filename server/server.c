#include <stdio.h>
#include <unistd.h>
#include "consumers.h"

task_attr *tk; // pointer to the array of task_attr

unsigned int C[]; /** It contains runtime for each task */
unsigned int D[]; /** It contains deadline for each task(equal server deadline) */
unsigned int T[]; /** It contains period for each server */

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

  // generates configurations for consumers threads

  // creates consumers & dispatcher

  // father assigns cpu to dispatcher and consumers,then suspends

  /* consumer1 creating n thread
   * send a signal_ready to dispatcher
   *
   */

  // consumer2 like consumer1

}
