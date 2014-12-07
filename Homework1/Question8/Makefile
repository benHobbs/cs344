# Name:				Benjamin Hobbs
# Email:            	hobbsbe@onid.oregonstate.edu
# Class:           	CS344-400
# Assignment: 	Homework #1

#
#  $RCSfile$
#  $Revision$
#  $Author$
#  $Date$
#  $Log$
#

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -lm
PROGS=sieveOfEratosthenes

all: $(PROGS)

sieveOfEratosthenes: sieveOfEratosthenes.o
	$(CC) $(CFLAGS) -o sieveOfEratosthenes sieveOfEratosthenes.o

sieveOfEratosthenes.o: sieveOfEratosthenes.c
	$(CC) $(CFLAGS) -c sieveOfEratosthenes.c

clean:
	rm -f $(PROGS) *.o *~

