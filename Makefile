comp=gcc -std=c99 -Wall
path=bin

lib=-lncurses

clientobj=	\
		$(path)/client.o	\
		$(path)/message.o

serverobj=	\
		$(path)/server.o	\
		$(path)/message.o

all : $(path)/client $(path)/server

$(path)/client : $(clientobj)
	$(comp) $(clientobj) -o $(path)/client $(lib)

$(path)/server : $(serverobj)
	$(comp) $(serverobj) -o $(path)/server $(lib)

$(path)/%.o : %.c
	$(comp) -c $< -o $@

clean :
	rm -rf $(path)/*
