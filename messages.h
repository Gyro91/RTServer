#include <stdio.h>
#include <unistd.h>

enum msg_type { TYPE1, TYPE2 }; /** number of message type  */

/**
 * @brief Defines a message format:
 *
 * */

typedef struct message {
	enum msg_type type; /** type of the message */
	char *addr; /** destination address */
	int dport; /** destination port */
}dispatch;




