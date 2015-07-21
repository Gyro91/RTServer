#include <stdio.h>
#include <unistd.h>

/** number of message type  */

enum msg_type { TYPE1, TYPE2 };

/**
 * @brief Defines a message format
 *
 * */

typedef struct message_t_ {
	char *payload;
	enum msg_type type; /** type of the message */

} message_t;




