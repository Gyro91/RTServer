#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define DIM_MAX_PAYLOAD 56  /** @brief dim max message */
#define SCHED_DEADLINE	6
#define QUEUE_NAME1  "/queue1"
#define QUEUE_NAME2  "/queue2"
#define MAX_SIZE    1024


#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

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


/** number of message type  */
enum msg_type { TYPE1, TYPE2 };

/**
 * @brief Defines a message format
 * */

typedef struct message_t_ {
	enum msg_type type; /** type of the message */
	unsigned char size; /** size of payload */
	struct timespec arrival_time; /** arrival time packet to server */
} message_t;
