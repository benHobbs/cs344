#	Name:					Benjamin Hobbs
#	Email:            		hobbsbe@onid.oregonstate.edu
#	Class:           		CS344-400
#	Assignment: 		Homework #6

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -std=c99 -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
XFLAGS=-lrt -lm
PROGS=compute

all: $(PROGS)

compute: compute.o
	$(CC) $(CFLAGS) $(XFLAGS) -o compute compute.c

compute.o: compute.c
	$(CC) $(CFLAGS) $(XFLAGS) -c compute.c
	
clean:
	rm -f $(PROGS) *.o *~