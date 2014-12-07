/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:			Homework #6
 **************************************************************************************************
 ** Date Created:		8/26/2014
 ** File Name:			compute.c
 ** Overview:			...
 ** Citations:				https://kb.iu.edu/d/afnz
 **							http://en.wikipedia.org/wiki/Perfect_number
**************************************************************************************************/
#define _POSIX_C_SOURCE 199309
#define _POSIX_SOURCE
#define _BSD_SOURCE


#include <ar.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

#define LISTEN_PORT 8989	//port where the thread listens for KILL_SIGNAL
#define KILL_SIGNAL 0xBD

//void *signal_listener(void *v);

int main(int argc, char **argv){
	
	//pthread_t *listener_thread;	//thread that listens for kill signal
	//uintptr_t n = 0;

	unsigned int n = 0;
	unsigned int sum = 0;
	
	while (1) {
		sum = 0;
		for ( unsigned int i = 1 ; i < n ; i++ ) {
			if ( n % i == 0 ) {
				sum += i;
			}
		}
		if (sum == n ) {
			printf ( "%d\n" , n );
		}
		n++;
	}
	
	//return 0;
}