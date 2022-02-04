CC	= gcc
CFLAGS	= -g -Wall
PROGS	= project_4

all:		$(PROGS)

clean:;		$(RM) $(PROGS) *.o core

project_4:		utility.o project_4.o
		$(CC) -o project_4 utility.o project_4.o
