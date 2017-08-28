QUIET_SUBDIR0 = +$(MAKE) -C # space to separate -C and subdir
QUIET_SUBDIR1 =
PRINT_DIR = --no-print-directory
ifndef V
	QUIET_CC       = @echo ' ' CC '     ' $@;
	QUIET_BUILT_IN = @echo ' ' BUILTIN '' $@;
	QUIET_CLEAN    = @echo ' ' CLEAN '  ' $<;
	QUIET_SUBDIR0  = +@subdir=
	QUIET_SUBDIR1  = ;$(NO_SUBDIR) echo ' ' SUBDIR ' ' $$subdir; \
		$(MAKE) $(PRINT_DIR) -C $$subdir
endif

CC = cc
CFLAGS = -std=c99
RM = rm -f

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
	$(QUIET_BUILT_IN)$(CC) $(clientobj) -o $(path)/client $(clientlib)


$(path)/server : $(serverobj)
	$(QUIET_BUILT_IN)$(CC) $(serverobj) -o $(path)/server $(pathlib) $(serverlib)


libList :
	$(QUIET_SUBDIR0)List $(QUIET_SUBDIR1) all


$(path)/%.o : %.c
	$(QUIET_CC)$(CC) -c $< -o $@


check_bin :
	@ if [ ! -d $(path) ]; then mkdir $(path); fi

clean : $(path)
	$(QUIET_SUBDIR0)List $(QUIET_SUBDIR1) clean || \
	$(QUIET_CLEAN)$(RM) -r $(path)
