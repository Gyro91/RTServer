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
	srv_addr.sin_port = htons(1235);
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
void generate_message(message_t *mess, unsigned char size)
{
	mess->payload = (char *) malloc(size); // allocating memory for size bytes
	generate_payload(mess->payload,size); // random payload

	mess->type = draw_message(); // random type
}

/** @brief send a packet to the  server
 *  @param mess message to be delivered
 *  @param size dimension of the content(payload)
 */
void send_pkt(message_t *mess,unsigned char size)
{
	int ret;
	char *buffer;

	buffer = (char *)malloc(size + 1);
	strcpy(buffer,mess->payload);
	if( mess->type )
		strcat(buffer,"1");
	else
		strcat(buffer,"0");

	size = size + 1;
	// send dimension
	printf("%s\n", buffer);
	printf("%d\n", size);

	ret = send(sk,(void *)&size,1,0);
	if( ret < 0 )
		Error_("Error send dimension");

	// send message
	ret = send(sk,(void *)buffer,size,0);
	if( ret < 0 )
		Error_("Error send message!");

	/*
	int i;
	for(i = 0; i < size ; ++i){
		ret = send(sk,(void *)&buffer[i], 1 ,0);
		if( ret < 0 )
			Error_("Error send message");
	}
	*/
	free(buffer);
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
