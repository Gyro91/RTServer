#define _GNU_SOURCE
#include <time.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>
#include <pthread.h>

/** @brief calculates response time in microseconds */

long int calculate_pt(struct timespec *ta, struct timespec *tf)
{
	long int at, ft;

	at = (ta->tv_sec * 1000  * 1000 ) + (ta->tv_nsec / 1000);
	ft = (tf->tv_sec * 1000  * 1000) + (tf->tv_nsec / 1000);

	return (ft - at);
}

/** add ms to the specific destination */

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

void time_copy(struct timespec *dst, const struct timespec *src)
{
  dst->tv_sec = src->tv_sec;
  dst->tv_nsec = src->tv_nsec;
}


struct sched_attr {
	__u32 size;

	__u32 sched_policy;
	__u64 sched_flags;

	/* SCHED_NORMAL, SCHED_BATCH */
	__s32 sched_nice;

	/* SCHED_FIFO, SCHED_RR */
	 __u32 sched_priority;

	/* SCHED_DEADLINE (nsec) */
	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
};


int sched_setattr(pid_t pid,
				  const struct sched_attr *attr,
			   	  unsigned int flags)
{
	return syscall(__NR_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid,
				  struct sched_attr *attr,
		          unsigned int size,
		          unsigned int flags)
{
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
}
