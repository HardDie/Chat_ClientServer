#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define MESSAGE_SIZE 5000
#define NAME_SIZE 100

typedef struct usMessage_t {
	char	user_[NAME_SIZE];
	char	msg_[MESSAGE_SIZE];
} usMessage_t;

#endif
