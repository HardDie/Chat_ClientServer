#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "message.h"

struct usUser_t {
    int                     socket_;
    char                    username_[NAME_SIZE];
    pthread_t               thread_;
};

enum state_t {
	STATE_MENU,
	STATE_REGISTRATION,
	STATE_LOGIN,
	STATE_CHAT,
	STATE_EXIT
};

#endif
