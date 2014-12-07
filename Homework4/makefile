#	Name:					Benjamin Hobbs
#	Email:            		hobbsbe@onid.oregonstate.edu
#	Class:           		CS344-400
#	Assignment: 		Homework #4

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -std=c99 -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
PROGS=pipeline uniqify

all: $(PROGS)

pipeline: pipeline.o
	$(CC) $(CFLAGS) -o pipeline pipeline.c

pipeline.o: pipeline.c
	$(CC) $(CFLAGS) -c pipeline.c
	
uniqify: uniqify.o
	$(CC) $(CFLAGS) -o uniqify uniqify.c

uniqify.o: uniqify.c
	$(CC) $(CFLAGS) -c uniqify.c
	
clean:
	rm -f $(PROGS) *.o *~
	rm -f myuniqify_t uniqify_t
	rm -f pipe1 pipe2 test_pipe1 test_pipe2
	
test_pipeline:
	rm -f pipe1 pipe2 test_pipe1 test_pipe2
	sample/pipeline.bash -f test_pipe2 < sample/words.txt > test_pipe1
	./pipeline -f pipe2 < sample/words.txt > pipe1
	diff pipe1 test_pipe1
	diff pipe2 test_pipe2

test_uniqify:
	rm -f myuniqify_t uniqify_t
	sample/./uniquify.bash < sample/websters.txt > uniqify_t
	./uniqify -n 5 < sample/websters.txt > myuniqify_t
	diff uniqify_t myuniqify_t

test_uniqify_decl:
	rm -f myuniqify_t uniqify_t
	sample/./uniquify.bash < sample/decl-indep.txt > uniqify_t
	./uniqify -n 5 < sample/decl-indep.txt > myuniqify_t
	diff uniqify_t myuniqify_t

test: test_pipeline test_uniqify
