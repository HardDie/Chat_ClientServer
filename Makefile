comp=gcc -std=c99 -Wall
path=bin
pathlib=-Llib
clientlib=-lpthread -lncurses
serverlib=-lpthread -lList

lib=-lncurses

clientobj=	\
		$(path)/client.o

serverobj=	\
		$(path)/server.o	\
		$(path)/network.o

all : check_bin $(path)/client $(path)/server

$(path)/client : $(clientobj)
	$(comp) $(clientobj) -o $(path)/client $(clientlib)

$(path)/server : $(serverobj)
	$(comp) $(serverobj) -o $(path)/server $(pathlib) $(serverlib)

$(path)/%.o : %.c
	$(comp) -c $< -o $@

check_bin :
	@ if [ ! -d $(path) ]; then mkdir $(path); fi

clean :
	rm -rf $(path)
