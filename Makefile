comp=gcc -std=c99 -Wall
path=bin
pathlib=-LList/bin
clientlib=-lpthread -lncurses
serverlib=-lpthread -lList

lib=-lncurses

clientobj=	\
		$(path)/client.o

serverobj=	\
		$(path)/server.o	\
		$(path)/network.o

all : libList check_bin $(path)/client $(path)/server

$(path)/client : $(clientobj)
	$(comp) $(clientobj) -o $(path)/client $(clientlib)

$(path)/server : $(serverobj)
	$(comp) $(serverobj) -o $(path)/server $(pathlib) $(serverlib)

libList : List
	cd List && $(MAKE)

List :
	git clone https://github.com/HardDie/List.git

$(path)/%.o : %.c
	$(comp) -c $< -o $@

check_bin :
	@ if [ ! -d $(path) ]; then mkdir $(path); fi

clean :
	rm -rf $(path)
	cd List && $(MAKE) clean
