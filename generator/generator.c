#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "messages.h"
#include <time.h>
#include <string.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#define TEST // for testing it needs variables that are defined in main.c
			 // but for the test it is necessary generator.o without linking

#ifdef TEST
struct sockaddr_in srv_addr; /** server address */
int sk; /* socket for communication between generator and server */
#endif TEST


/** @brief print the error passed and exit */

void Error_(char *string)
{
	perror(string);
	exit(1);
}

/**
  * assigns a core to the process
  */

void init_generator()
{
	cpu_set_t bitmap;

	CPU_ZERO(&bitmap); // resetting bitmap
	CPU_SET(0, &bitmap); // setting bitmap to zero

	sched_setaffinity(0, sizeof(bitmap), &bitmap); // taking cpu-0
}



/** @brief decides randomly what kind of message to forward
 *  @return type of message
 * */
enum msg_type draw_message()
{
	int random = rand() % 2;

	if( random )
		return TYPE1;
	else
		return TYPE2;
}


/** setup a TCP connection like a client */

void setup_TCP_client()
{
	int ret;

	sk = socket(AF_INET, SOCK_STREAM, 0);

	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(1234);
	ret = inet_pton(AF_INET, "192.168.1.1", &srv_addr.sin_addr);

	ret = connect(sk, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	if( ret < 0 )
		Error_("Error connect");
}

/**  @brief generates randomly a payload
 * 	 @param pointer to payload
 *   @return 1 in case of success,0 vice versa
 */

void generate_payload(char *payload, int size)
{
	int i;
	char c;

	srand(time(NULL)); // setting seed

	for(i = 0; i < size-1; i++){
		c = rand() % 65;;
		c += 65;
		payload[i] = c;
	}

	payload[size-1] = '\0';

}

/** @brief generate a message
 *	@param mess struct to contain mess
 *	@param pay is the payload of the message
 */
// must be modified (message_t,size)
void generate_message(message_t *mess)
{
	int size;

	srand(time(NULL));

	size = (rand() % ( DIM_MAX_PAYLOAD + 1 )) + 1; // dim between 1 & DIM_MAX
	mess->payload = (char *) malloc(size); // allocating memory for size bytes
	generate_payload(mess->payload,size);

	mess->type = draw_message();
}

