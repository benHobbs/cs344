/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #3
 **************************************************************************************************
 ** Date Created:		7/12/2014
 ** File Name:			sig_demo.c
 ** Overview:			Program to send and receives unix signals.
 ** Citations:				http://man7.org/linux/man-pages/man2/getpid.2.html
 **							http://www.cyberciti.biz/faq/unix-kill-command-examples/
 **							http://en.cppreference.com/w/c/program/signal
**************************************************************************************************/
#define _POSIX_SOURCE 
#include <stdio.h>
#include <getopt.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

static void sigHandler_1(int sig)
{
	printf("SIGUSR1 has been caught\n");
}

static void sigHandler_2(int sig)
{
	printf("SIGUSR2 has been caught\n");
}

static void sigHandler_3(int sig)
{
	printf("SIGINT has been caught, terminating the program\n");
}

//int kill(pid_t pid, int sig);

 /*************************************************************************************************
 ** Function:			main
 ** Description:		Uses getopt to run some system calls
 ** Precondition:		command line args -h, -t , or -f filename
 ** Postcondition:		n/a
 *************************************************************************************************/
int main(int argc, char *argv[])
{
	if (signal(SIGUSR1, sigHandler_1) == SIG_ERR) {
		printf("Signal Error");
		return -1;
	}
	
	if (signal(SIGUSR2, sigHandler_2) == SIG_ERR) {
		printf("Signal Error");
		return -1;
	}

	if (signal(SIGINT, sigHandler_3) == SIG_ERR) {
		printf("Signal Error");
		return -1;
	}
	
	kill(getpid(),10);
	kill(getpid(),12);
	kill(getpid(),2);
	
	return 0;
}
