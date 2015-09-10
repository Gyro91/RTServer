#define _GNU_SOURCE
#include <time.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>


#define gettid() syscall(__NR_gettid)
#define SCHED_DEADLINE	6

/* XXX use the proper syscall numbers */
#ifdef __x86_64__
#define __NR_sched_setattr		314
#define __NR_sched_getattr		315
#endif

#ifdef __i386__
#define __NR_sched_setattr		351
#define __NR_sched_getattr		352
#endif

#ifdef __arm__
#define __NR_sched_setattr		380
#define __NR_sched_getattr		381
#endif

#define QUEUE_NAME1  "/queue1"
#define QUEUE_NAME2  "/queue2"
#define MAX_SIZE    1024
#define DIM_MAX_PAYLOAD 56

#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

/** number of message type  */


enum msg_type { TYPE1, TYPE2 };


/** @brief Defines a message format */


typedef struct message_t_ {
	enum msg_type type; /** type of the message */
	unsigned char size; /** size of payload */
	struct timespec arrival_time; /** arrival time packet to server */
} message_t;


/** element for recording processing task */


struct processing_task {
	pthread_t tid; /** thread-id for the task */
	struct timespec arrival_time;
	struct timespec finishing_time;

};



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
                  unsigned int flags);

int sched_getattr(pid_t pid,
                  struct sched_attr *attr,
                  unsigned int size,
                  unsigned int flags);

void time_add_ms(struct timespec *dst, long int ms);

long int calculate_pt(struct timespec *ta, struct timespec *tf);

void time_copy(struct timespec *dst, const struct timespec *src);
