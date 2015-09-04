#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#include "messages.h"

int type = 0;
/*
 * In testing mode global variable,besides it looks in another file
 * */
#ifdef TEST
struct sockaddr_in srv_addr; /** server address */
int sk; /* socket for communication between generator and server */
#endif

#ifndef TEST
extern struct sockaddr_in srv_addr; /** server address */
extern int sk; /* socket for communication between generator and server */
#endif

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
	if( type == 0 ){
		type = 1;
		return TYPE1;
	}
	else {
		type = 0;
		return TYPE2;
	}

}


/** setup a TCP connection like a client */

void setup_TCP_client()
{
	int ret;

	sk = socket(AF_INET, SOCK_STREAM, 0);

	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(1296);
	ret = inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr);

	ret = connect(sk, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	if( ret < 0 )
		Error_("Error connect");

    printf("#Connected with server\n");
}

/**  @brief generates randomly a payload
 * 	 @param pointer to payload
 *   @return 1 in case of success,0 vice versa
 */

void generate_payload(char *payload, unsigned char size)
{
	int i;
	char c;

	srand(time(NULL)); // setting seed

	for(i = 0; i < size-1; i++){
		c = rand() % 26;;
		c += 97;
		payload[i] = c;
	}

	payload[size-1] = '\0';

}

/** @brief generate a message
 *
 */

void generate_message(message_t *mess)
{
	mess->type = draw_message(); // random type
	mess->size = (rand() % ( DIM_MAX_PAYLOAD + 1 )) + 8; // dim between 8 & DIM_MAX

}

/** @brief send a packet to the  server
 *  @param mess message contains info to be delivered
 *  @param payload is the content of the message
 */
void send_pkt(message_t *mess, char *payload)
{
	int ret;

	printf("Type: %d\nPayload: %s\n\n", mess->type, payload);

	// send struct
	ret = send(sk, (void *)mess, sizeof(message_t), 0);
	if( ret < 0 )
		Error_("Error send message");

	// send message
	ret = send(sk, (void *)payload, mess->size, 0);
	if( ret < 0 )
		Error_("Error send payload!");

	/*
	int i;
	for(i = 0; i < size ; ++i){
		ret = send(sk,(void *)&buffer[i], 1 ,0);
		if( ret < 0 )
			Error_("Error send message");
	}
	*/

}

/** Add micro seconds to the specific destination */

void time_add_micros(struct timespec *dst, long int ms)
{
  dst->tv_sec += ms/1000000;
  dst->tv_nsec += (ms % 1000000) * 1e3;

  if (dst->tv_nsec > 1e9) {
	  dst->tv_nsec -= 1e9;
	  dst->tv_sec++;
  }
}
