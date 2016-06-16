#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>  // struct hostent

#include "lib/List.h"
#include "message.h"
#include "network.h"
#include "global.h"

usList_t *              list;
struct usUser_t *		tmpUser;

void * client_scocket_reader ( void * userStruct ) {
	char 				password[NAME_SIZE];	// Временное хранилище пароля
    struct usUser_t * 	user = userStruct;	// Структура хранящая описание пользователя
	usMessage_t 		msg;	// Структура сообщения

	printf( "Connection created!\n" );
    enum state_t state;
    state = STATE_MENU;

	while ( 1 ) {
	    switch ( state ) {
	        case STATE_MENU:
				strcpy( msg.msg_, "If you want registration(!r), login(!l)" );
				strcpy( msg.user_, "Server" );
				NetSendMessage( user, &msg );
	            if ( NetRecieveMessege( user, &msg ) == 0 ) {
	        			state = STATE_EXIT;
						fprintf( stderr, "Client disconnected\n" );
						break;
	        	}
				if ( strcmp( msg.msg_, "!r" ) == 0 ) {
					state = STATE_REGISTRATION;
					break;
				} else if ( strcmp( msg.msg_, "!l" ) == 0 ) {
					state = STATE_LOGIN;
					break;
				}
	            break;
	        case STATE_REGISTRATION:
				/* Login */
				strcpy( msg.msg_, "Enter your login" );
				strcpy( msg.user_, "Server" );
				NetSendMessage( user, &msg );
				if ( NetRecieveMessege( user, &msg ) == 0 ) {
						state = STATE_EXIT;
						fprintf( stderr, "Client disconnected\n" );
						break;
				}
				if ( strlen( msg.msg_ ) >= NAME_SIZE ) {
					for ( int i = 0; i < NAME_SIZE - 1; i++ ) {
						user->username_[i] = msg.msg_[i];
					}
					user->username_[NAME_SIZE - 1] = '\0';
				} else {
					strcpy( user->username_, msg.msg_ );
				}
				/* Password */
				strcpy( msg.msg_, "Enter your password" );
				NetSendMessage( user, &msg );
				if ( NetRecieveMessege( user, &msg ) == 0 ) {
						state = STATE_EXIT;
						fprintf( stderr, "Client disconnected\n" );
						break;
				}
				if ( strlen( msg.msg_ ) >= NAME_SIZE ) {
					for ( int i = 0; i < NAME_SIZE - 1; i++ ) {
						password[i] = msg.msg_[i];
					}
					password[NAME_SIZE - 1] = '\0';
				} else {
					strcpy( password, msg.msg_ );
				}
				ClearBuffer( password, NAME_SIZE );
				state = STATE_CHAT;
	            break;
	        case STATE_LOGIN:
				/* Login */
				strcpy( msg.msg_, "Enter your login" );
				strcpy( msg.user_, "Server" );
				NetSendMessage( user, &msg );
				if ( NetRecieveMessege( user, &msg ) == 0 ) {
						state = STATE_EXIT;
						fprintf( stderr, "Client disconnected\n" );
						break;
				}
				if ( strlen( msg.msg_ ) >= NAME_SIZE ) {
					for ( int i = 0; i < NAME_SIZE - 1; i++ ) {
						user->username_[i] = msg.msg_[i];
					}
					user->username_[NAME_SIZE - 1] = '\0';
				} else {
					strcpy( user->username_, msg.msg_ );
				}
				/* Password */
				strcpy( msg.msg_, "Enter your password" );
				NetSendMessage( user, &msg );
				if ( NetRecieveMessege( user, &msg ) == 0 ) {
						state = STATE_EXIT;
						fprintf( stderr, "Client disconnected\n" );
						break;
				}
				if ( strlen( msg.msg_ ) >= NAME_SIZE ) {
					for ( int i = 0; i < NAME_SIZE - 1; i++ ) {
						password[i] = msg.msg_[i];
					}
					password[NAME_SIZE - 1] = '\0';
				} else {
					strcpy( password, msg.msg_ );
				}
				ClearBuffer( password, NAME_SIZE );
				state = STATE_CHAT;
	            break;
			case STATE_CHAT:
				strcpy( msg.user_, "Server" );
				strcpy( msg.msg_, user->username_ );
				strcat( msg.msg_, ", welcome to chat!" );
				NetSendMessage( user, &msg );
				while ( 1 ) {
					if ( NetRecieveMessege( user, &msg ) == 0 ) {
							state = STATE_EXIT;
							fprintf( stderr, "Client [%s] disconnected\n", user->username_ );
							break;
					}
					strcpy( msg.user_, user->username_ );
					if ( strcmp( msg.msg_, "!logout" ) == 0 ) {
						state = STATE_EXIT;
						fprintf( stderr, "Client [%s] leave chat\n", user->username_ );
						break;
					} else {
						for ( int i = 0; i < ListSize( &list ); i++ ) {
							NetSendMessage( ListGetElement( &list, i ), &msg );
						}
					}
				}
				break;
	        case STATE_EXIT:
				close( user->socket_ );
				ListDeleteElementAtData( &list, user );
				return NULL;
	            break;
	        default:
	            break;
	    }
	}

    return NULL;
}



int main( int argc, char *argv[] ) {
	NetStartServer();

    while ( 1 ) {
        tmpUser = malloc( sizeof( struct usUser_t ) );
        tmpUser->socket_ = NetAcceptConnection();
        if ( tmpUser->socket_ < 0 ) {
            fprintf( stderr, "ERROR conection client\n" );
            free( tmpUser );
        } else {
            pthread_create ( &( tmpUser->thread_ ), NULL, client_scocket_reader, tmpUser );
            ListPushBack( &list, tmpUser );
        }
    }

	for( int i = 0; i < ListSize( &list ); i++ ) {
		close( ( ( struct usUser_t * )ListGetElement( &list, i ) )->socket_ );
	}

    ListFree( &list );
    return 0;
}
