CC	= gcc
CFLAGS	= -g -Wall -pthread
PROGS	= sim

all:		$(PROGS)

clean:;		$(RM) $(PROGS) *.o core

sim:		sim.o
		$(CC) $(CFLAGS) -o sim sim.o
