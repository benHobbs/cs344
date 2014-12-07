#	Name:					Benjamin Hobbs
#	Email:            		hobbsbe@onid.oregonstate.edu
#	Class:           		CS344-400
#	Assignment: 		Homework #5

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -std=c99 -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
XFLAGS=-lrt -lm
PROGS=prime_mproc prime_pthread

all: $(PROGS)

prime_mproc: prime_mproc.o
	$(CC) $(CFLAGS) $(XFLAGS) -o prime_mproc prime_mproc.c

mproc.o: prime_mproc.c
	$(CC) $(CFLAGS) $(XFLAGS) -c prime_mproc.c
	
prime_pthread: prime_pthread.o
	$(CC) $(CFLAGS) $(XFLAGS) -o prime_pthread prime_pthread.c

pthread.o: prime_pthread.c
	$(CC) $(CFLAGS) $(XFLAGS) -c prime_pthread.c
	
clean:
	rm -f $(PROGS) *.o *~