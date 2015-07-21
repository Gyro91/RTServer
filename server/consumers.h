#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct task_attr_ {

  unsigned int dmiss;		// number of deadline misses
  unsigned int index;       // to locate the struct-i for task i

  long int deadline;		// task deadline

  long int s_period;		// server period
  long int s_deadline;	// server deadline
  long int s_runtime;	// server runtime that will be used
                        // for bandwidth management

} task_attr;
