CC=gcc 
AR=ar
MAINOBJECTS=main.o
FLAGS= -Wall -g -pthread

all: 2.1.1 2.1.2 2.2.1 2.2.2 2.3.1 2.3.2 dining_philosophers

2.1.1: 2.1.1.c
	$(CC) $(FLAGS) 2.1.1.c -o 2.1.1
2.1.2: 2.1.2.c
	$(CC) $(FLAGS) 2.1.2.c -o 2.1.2
2.2.1: 2.2.1.c
	$(CC) $(FLAGS) 2.2.1.c -o 2.2.1
2.2.2: 2.2.2.c
	$(CC) $(FLAGS) 2.2.2.c -o 2.2.2
2.3.1: 2.3.1.c
	$(CC) $(FLAGS) 2.3.1.c -o 2.3.1
2.3.2: 2.3.2.c
	$(CC) $(FLAGS) 2.3.2.c -o 2.3.2
dining_philosophers: dining_philosophers.c
	$(CC) $(FLAGS) dining_philosophers.c -o dining_philosophers

.PHONY: clean all

clean:
	rm -f *.o 2.1.1 2.1.2 2.2.1 2.2.2 2.3.1 2.3.2
