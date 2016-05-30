#include <ncurses.h>
#include <string.h>

#include "message.h"

WINDOW		*		winChat = NULL;
WINDOW		*		winMsg = NULL;

int widthChatWindow;	// Ширина окна чата
int heigthChatWindow;	// Высота окна чата
int widthMsgWindow;		// Ширина окна ввода сообщения
int heightMsgWindow;	// Высота окна ввода сообщения

void setupWindow();
void clearMemory();
void drawWindows();
void drawMessage();

int main() {
	setupWindow();
	drawWindows();

	char isDone = 0;

	while ( !isDone ) {
		wclear( winMsg );
		wprintw( winMsg, "Enter message: " );
		wgetstr( winMsg, message.msg_ );
		strcpy( message.user_, "You" );
		if ( strcmp( "exit", message.msg_ ) == 0 || strcmp( "q", message.msg_ ) == 0 ) {
			isDone = 1;
		}
		drawMessage();
		wrefresh( winMsg );
		wrefresh( winChat );
	}

	clearMemory();
	return 0;
}

/*
 *	Name: setupWindow
 *	Description: Инициализирует ncurses, создает дочерние окна для вывода и ввода сообщений, задаются размеры окон
 */
void setupWindow() {
	initscr();
	start_color();

	widthChatWindow = COLS - 2;
	widthMsgWindow = COLS - 2;

	heigthChatWindow = ( LINES - 4 ) * 0.9;
	heightMsgWindow = ( LINES - 4 ) * 0.1;

	winChat = subwin( stdscr, heigthChatWindow, widthChatWindow, 2, 1 );
	winMsg = subwin( stdscr, heightMsgWindow, widthMsgWindow, heigthChatWindow + 3, 1 );

	scrollok( winChat, TRUE );	// Позволяет прокручивать экран если нет больше места
	scrollok( winMsg, TRUE );

	init_pair( 1, COLOR_WHITE, COLOR_BLACK );
	init_pair( 2, COLOR_GREEN, COLOR_BLACK );
}

/*
 *	Name: clearMemory
 *	Description: Очищает память от созданных окон и закрывает ncurses
 */
void clearMemory() {
	delwin( winChat );
	delwin( winMsg );
	endwin();
}

/*
 *	Name: drawWindows
 *	Description: Отрисовывает рамки вокруг окон и название приложения
 */
void drawWindows() {
	mvprintw( 0, COLS / 2 - 6, "ChatMessenger" );
	move( 1, 0 );
	for ( int i = 0; i < widthChatWindow + 2; i++ ) {
		addch( '-' );
	}

	move( heigthChatWindow + 2, 0 );
	for ( int i = 0; i < widthChatWindow + 2; i++ ) {
		addch( '-' );
	}

	move( 2, 0 );
	for ( int i = 0; i < heigthChatWindow; i++ ) {
		addch( '|' );
		for ( int j = 0; j < widthChatWindow; j++ ) {
			addch( ' ' );
		}
		addch( '|' );
	}

	move( heigthChatWindow + 3, 0 );
	for ( int i = 0; i < heightMsgWindow; i++ ) {
		addch( '|' );
		for ( int j = 0; j < widthMsgWindow; j++ ) {
			addch( ' ' );
		}
		addch( '|' );
	}
	for ( int i = 0; i < widthMsgWindow + 2; i++ ) {
		addch( '-' );
	}
	refresh();
}

/*
 *	Name: drawMessage
 *	Description: Выводит сообщение
 */
void drawMessage() {
	wattron( winChat, COLOR_PAIR( 2 ) );	// Меняем цвет для вывода никнейма
	wprintw( winChat, "\n%s: ", message.user_ );
	wattron( winChat, COLOR_PAIR( 1 ) );	// Возвращаем стандартный цвет
	wprintw( winChat, "%s", message.msg_ );
	wrefresh( winChat );
}
