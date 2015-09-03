#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "messages.h"


#ifndef FUN_H
#define FUN_H
extern enum msg_type draw_message(void);
extern void init_generator(void);
extern void generate_payload(char *, unsigned char);
extern void generate_message(message_t *);
extern void setup_TCP_client(void);
extern void time_add_micros(struct timespec *, long int);
extern void send_pkt(message_t *, char *);
#endif
