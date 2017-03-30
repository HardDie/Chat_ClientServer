#include "network.h"

unsigned short    	    port = 1666;
int					    sock_server;
struct sockaddr_in 	    server_addr;

/*
====================
NetStartServer

	Запускает сервер, в случае ошибки возвращает -1
====================
*/
int NetStartServer() {
	server_addr.sin_family = AF_INET;		// Инициализация параметров
 	server_addr.sin_port = htons( port );
 	bzero( &( server_addr.sin_zero ), 8 );

    int val_true = 1;

    if ( ( sock_server = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {		// Создаем сокет
        fprintf( stderr, "ERROR server cannot create socket\n" );
		return -1;
    }

    if ( setsockopt( sock_server, SOL_SOCKET, SO_REUSEADDR, &val_true, sizeof( int ) ) == -1 ) {
        close( sock_server );
        fprintf( stderr, "ERROR setsockopt\n" );
		return -1;
    }

    server_addr.sin_addr.s_addr = INADDR_ANY;

    if ( bind( sock_server, ( struct sockaddr * )&server_addr, sizeof( struct sockaddr ) ) == -1 ) {
        close( sock_server );
        fprintf( stderr, "ERROR unable to bind\n" );
		return -1;
    }
    if ( listen( sock_server, 1 ) == -1 ) {
        close( sock_server );
        fprintf( stderr, "ERROR listen\n" );
		return -1;
    }
	return 0;
}

/*
====================
NetAcceptConnection

	Принимает соединение с клиентом, возвращает дескриптор клиента
====================
*/
int	NetAcceptConnection() {
	struct sockaddr_in cli_addr;
	int sin_size = sizeof( struct sockaddr_in );
	return accept( sock_server, ( struct sockaddr * )&( cli_addr ), ( socklen_t * )&sin_size );
}

/*
====================
NetRecieveMessege

	Принимает сообщение от клиента, возвращает результат работы функции
====================
*/
int	NetRecieveMessege( struct usUser_t * user, usMessage_t * msg ) {
	ClearBuffer( msg->msg_, MESSAGE_SIZE );
	return recv( user->socket_, &msg->msg_, MESSAGE_SIZE, 0 );
}

/*
====================
NetSendMessage

	Отправляет сообщение клиенту
====================
*/
void NetSendMessage( struct usUser_t * user, usMessage_t * msg ) {
	send( user->socket_, msg, sizeof( usMessage_t ), 0 );
}

/*
====================
ClearBuffer

	Очищает массив
====================
*/
void ClearBuffer( char *buf, int size ) {
    for ( int i = 0; i < size; i++ ) {
        buf[i] &= 0;
    }
}
