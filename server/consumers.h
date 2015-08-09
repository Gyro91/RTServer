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


struct response_task {

	pid_t tid; /** thread-id for the task */
	struct timespec arrival_time;
	struct timespec finishing_time;

};


/* add ms to the specific destination */
void time_add_ms(struct timespec *dst, long int ms)
{
	dst->tv_sec += ms/1000;
	dst->tv_nsec += (ms % 1000) * 1e6;

	if (dst->tv_nsec > 1e9) {
		dst->tv_nsec -= 1e9;
		dst->tv_sec++;
	}
}

/*
 * Copies the second passed time to the first one
 */
inline void time_copy(struct timespec *dst, const struct timespec *src)
{
  dst->tv_sec = src->tv_sec;
  dst->tv_nsec = src->tv_nsec;
}
