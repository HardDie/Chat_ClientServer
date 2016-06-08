#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>  // struct hostent

#include "message.h"
#include "global.h"

int		NetStartServer();
int		NetAcceptConnection();
int		NetRecieveMessege( struct usUser_t * user, usMessage_t * msg );
void	NetSendMessage( struct usUser_t * user, usMessage_t * msg );
void 	ClearBuffer( char *buf, int size );

#endif
