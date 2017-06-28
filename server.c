#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>  // struct hostent
#include <mcheck.h>

#include "List/List.h"
#include "message.h"
#include "network.h"
#include "global.h"

usList_t *              list;
struct usUser_t *		tmpUser;
pthread_t				clientThread;

void * WorkWithClient ( void * userStruct );
void * ListenClients();

int main( int argc, char *argv[] ) {
	mtrace();
	/* Start server */
	NetStartServer();
	pthread_create( &clientThread, NULL, ListenClients, NULL );

	/* Server command line */
	printf( "Server created and wait clients, when show command enter \'help\'\n" );
	char buffer[1024];
	while( 1 ) {
		printf( "> " );
		scanf( "%s", buffer );
		if ( strcmp( buffer, "exit" ) == 0 ) {
			break;
		} else if ( strcmp( buffer, "list" ) == 0 ) {
			for ( int i = 0; i < ListSize( &list ); i++ ) {
				printf( "[ %s ] ", (( struct usUser_t * )ListGetElement( &list, i ))->username_ );
			}
			printf( "\n" );
		} else if ( strcmp( buffer, "help" ) == 0 ) {
			printf( "\texit - close server\n" );
			printf( "\tlist - show list clients\n" );
		}
	}

	/* Close server */
	for( int i = 0; i < ListSize( &list ); i++ ) {
		close( (( struct usUser_t * )ListGetElement( &list, i ))->socket_ );
	}

    ListFree( &list );
	free( tmpUser );
	muntrace();
    return 0;
}

/*
====================
ListenClients

	Слушает клиентов и добавляет их в список в случае подключения нового
====================
*/
void * ListenClients() {
	while ( 1 ) {
        tmpUser = malloc( sizeof( struct usUser_t ) );
        tmpUser->socket_ = NetAcceptConnection();
        if ( tmpUser->socket_ < 0 ) {
            fprintf( stderr, "ERROR conection client\n" );
            free( tmpUser );
			return NULL;
        } else {
            pthread_create ( &( tmpUser->thread_ ), NULL, WorkWithClient, tmpUser );
            ListPushBack( &list, tmpUser );
        }
    }
	return NULL;
}

/*
====================
WorkWithClient

	Осуществляет работу с отдельным клиентом
====================
*/
void * WorkWithClient ( void * userStruct ) {
	char 				password[NAME_SIZE];	// Временное хранилище пароля
    struct usUser_t * 	user = userStruct;	// Структура хранящая описание пользователя
	usMessage_t 		msg;	// Структура сообщения

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
