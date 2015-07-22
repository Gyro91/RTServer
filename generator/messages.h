#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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




