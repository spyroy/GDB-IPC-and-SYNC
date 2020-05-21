CC=gcc 
AR=ar
MAINOBJECTS=main.o
LIBOBJECTS=power.o basicMath.o myMath.h
FLAGS= -Wall -g 

all: IPC

IPC.o: IPC.c
	$(CC) $(FLAGS) -c IPC.c

.PHONY: clean all

clean:
	rm -f *.o 
