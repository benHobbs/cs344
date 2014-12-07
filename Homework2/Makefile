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
PROGS=problem7

all: $(PROGS)

problem7: problem7.o
	$(CC) $(CFLAGS) -o problem7 problem7.o

problem7.o: problem7.c
	$(CC) $(CFLAGS) -c problem7.c

clean:
	rm -f $(PROGS) *.o *~

