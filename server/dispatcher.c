#include <stdio.h>
#include <unistd.h>
#include "messages.h"
#define LOOP 1


/** @brief waits a signal from both consumers
 *
 *  when consumers have set up, it allows dispatcher to do its task
 *
 * */

void wait_for_signal()
{


}

/** @brief receive packet from generator
 *
 *  it receives size,then the packet and generates a message_t
 *
 * 	@return original message,composed from type and payload
 */

message_t receive_pkt()
{
		// buffering
}

/**
 * @brief setup for listening
 *
 * it does prelimanry operations for a connection TCP
 */

void setup()
{

}


/**
 *  @brief forwards packet to a consumer
 *
 *  it forwards the message via IPC to the consumer
 *
 */
void forward_message(){


}

int main(int argc, char *argv[])
{	

	// waiting for a signal from consumers(condition variable)

	// setup for listening

	// accept

	while( LOOP ){
    
	// receive

	// decide those who to forward

	// forwarding

	// sleep to next period
	}

	return 0;

}
