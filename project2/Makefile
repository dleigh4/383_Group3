CC	= gcc
CFLAGS	= -g -Wall
PROGS	= sim

all:		$(PROGS)

clean:;		$(RM) $(PROGS) *.o core

sim:		sim.o proc.o node.o stat.o fcfs.o sjf.o srt.o rr.o hpf.o
		$(CC) -o sim sim.o proc.o node.o stat.o fcfs.o sjf.o srt.o rr.o hpf.o
