#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#define DIM_MAX_PAYLOAD 56  /** @brief dim max message */

/** number of message type  */
enum msg_type { TYPE1, TYPE2 };

/**
 * @brief Defines a message format
 * */

typedef struct message_t_ {
	char *payload;       /* content of the message */
	enum msg_type type; /** type of the message */

} message_t;


#ifndef FUN_H
#define FUN_H
extern enum msg_type draw_message(void);
extern void init_generator(void);
extern void generate_payload(char *, unsigned char);
extern void generate_message(message_t *, unsigned char);
extern void setup_TCP_client(void);
extern void time_add_ms(struct timespec *, long int);
#endif
