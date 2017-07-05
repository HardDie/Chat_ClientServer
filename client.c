#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <strings.h>
#include <netdb.h>
#include <pthread.h>

#include "message.h"

WINDOW* winChat = NULL;
WINDOW* winMsg = NULL;

int widthChatWindow;   // Ширина окна чата
int heigthChatWindow;  // Высота окна чата
int widthMsgWindow;    // Ширина окна ввода сообщения
int heightMsgWindow;   // Высота окна ввода сообщения

unsigned short port = 1666;  // Порт
char ip[16] = "localhost";   // IP
int sock_enemy;              // Дескриптор сокета сервера
pthread_t treadRead;  // Поток для приема сообщений
usMessage_t recievedMessage;
usMessage_t sendMessage;

pthread_mutex_t displayMutex;

void SetupWindow();
void ClearMemory();
void DrawWindows();
void DrawMessage();
void DrawSystemMesage(char* msg);
void* RecieveMessage();
void SendMessage();
int SetupNetwork();
void SetupMutex();
void ClearMutex();
void DrawEnterMessage();

int main() {
	SetupMutex();
	SetupWindow();
	DrawWindows();

	DrawSystemMesage("Start connection to server...");
	if (-1 == SetupNetwork()) {
		getch();
		ClearMemory();
		return -1;
	}
	DrawSystemMesage("Server connected");

	char isDone = 0;

	pthread_create(&treadRead, NULL, RecieveMessage, NULL);

	while (!isDone) {
		DrawEnterMessage();
		wgetnstr(winMsg, sendMessage.msg_, MESSAGE_SIZE - 1);
		SendMessage();
		if (strcmp("exit", sendMessage.msg_) == 0 ||
		    strcmp("q", sendMessage.msg_) == 0) {
			isDone = 1;
		}
	}

	close(sock_enemy);
	ClearMemory();
	ClearMutex();
	return 0;
}
/*
====================
SetupWindow

	Инициализирует ncurses, создает дочерние окна для вывода и ввода сообщений,
	задаются размеры окон
====================
*/
void SetupWindow() {
	initscr();
	start_color();

	widthChatWindow = COLS - 2;
	widthMsgWindow = COLS - 2;

	heigthChatWindow = (LINES - 4) * 0.9;
	heightMsgWindow = (LINES - 4) * 0.1;

	winChat = subwin(stdscr, heigthChatWindow, widthChatWindow, 2, 1);
	winMsg =
		subwin(stdscr, heightMsgWindow, widthMsgWindow, heigthChatWindow + 3, 1);

	// Позволяет прокручивать экран если нет больше места
	scrollok(winChat, TRUE);
	scrollok(winMsg, TRUE);

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
}

/*
====================
ClearMemory

	Очищает память от созданных окон и закрывает ncurses
====================
*/
void ClearMemory() {
	delwin(winChat);
	delwin(winMsg);
	endwin();
}

/*
====================
DrawWindows

	Отрисовывает рамки вокруг окон и название приложения
====================
*/
void DrawWindows() {
	mvprintw(0, COLS / 2 - 6, "ChatMessenger");
	move(1, 0);
	for (int i = 0; i < widthChatWindow + 2; i++) {
		addch('-');
	}

	move(heigthChatWindow + 2, 0);
	for (int i = 0; i < widthChatWindow + 2; i++) {
		addch('-');
	}

	for (int i = 2; i < heigthChatWindow + 2; i++) {
		move(i, 0);
		addch('|');
		move(i, widthChatWindow + 1);
		addch('|');
	}

	for (int i = heigthChatWindow + 3;
	     i < heigthChatWindow + 3 + heightMsgWindow;
	     i++) {
		move(i, 0);
		addch('|');
		move(i, widthMsgWindow + 1);
		addch('|');
	}
	for (int i = 0; i < widthMsgWindow + 2; i++) {
		addch('-');
	}
	refresh();
}

/*
====================
DrawMessage

	Выводит сообщение
====================
*/
void DrawMessage() {
	pthread_mutex_lock(&displayMutex);
	wattron(winChat, COLOR_PAIR(2));  // Меняем цвет для вывода никнейма
	wprintw(winChat, "\n%s: ", recievedMessage.user_);
	wattron(winChat, COLOR_PAIR(1));  // Возвращаем стандартный цвет
	wprintw(winChat, "%s", recievedMessage.msg_);
	wrefresh(winChat);
	wrefresh(winMsg);
	pthread_mutex_unlock(&displayMutex);
}

/*
====================
DrawSystemMesage

	Выводит системное сообщение
====================
*/
void DrawSystemMesage(char* msg) {
	pthread_mutex_lock(&displayMutex);
	wattron(winChat, COLOR_PAIR(3));  // Меняем цвет для вывода никнейма
	wprintw(winChat, "\nSYSTEM: %s", msg);
	wattron(winChat, COLOR_PAIR(1));  // Возвращаем стандартный цвет
	wrefresh(winChat);
	wrefresh(winMsg);
	pthread_mutex_unlock(&displayMutex);
}

/*
====================
RecieveMessage

	Принимает сообщения от сервера
====================
*/
void* RecieveMessage() {
	while (1) {
		memset(recievedMessage.msg_, 0, MESSAGE_SIZE);
		if (recv(sock_enemy, &recievedMessage, sizeof(usMessage_t), 0) == 0) {
			DrawSystemMesage("Server connection lost");
			break;
		}
		DrawMessage();
	}
	pthread_exit(NULL);
}

/*
====================
SendMessage

	Отправляет сообщение серверу
====================
*/
void SendMessage() {
	send(sock_enemy, &sendMessage.msg_, strlen(sendMessage.msg_), 0);
}

/*
====================
SetupNetwork

	Настраивает соединение с сервером, в случае ошибки возвращает -1
====================
*/
int SetupNetwork() {
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;  // Инициализация параметров
	server_addr.sin_port = htons(port);
	bzero(&(server_addr.sin_zero), 8);
	struct hostent *host;
	host = gethostbyname(ip);

	if (host == NULL) {
		DrawSystemMesage("Error, no such host");
		return -1;
	}

	// Создаем сокет
	if ((sock_enemy = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		DrawSystemMesage("Error create socket");
		return -1;
	}
	server_addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

	if (connect(sock_enemy, (struct sockaddr *)&server_addr,
	            sizeof(struct sockaddr)) == -1) {
		close(sock_enemy);
		DrawSystemMesage("Error connect to server");
		return -1;
	}
	return 0;
}

void SetupMutex() {
	if (pthread_mutex_init(&displayMutex, NULL) != 0) {
		fprintf(stderr, "Mutex failed init\n");
		exit(-1);
	}
}

void ClearMutex() {
	pthread_mutex_destroy(&displayMutex);
}

void DrawEnterMessage() {
	pthread_mutex_lock(&displayMutex);
	wclear(winMsg);
	wprintw(winMsg, "Enter message: ");
	wrefresh(winMsg);
	pthread_mutex_unlock(&displayMutex);
}
