/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #2
 **************************************************************************************************
 ** Date Created:		7/6/2014
 ** File Name:			problem7.c
 ** Overview:			Uses getopt to run some system calls
**************************************************************************************************/
 
#include <stdio.h>
#include <getopt.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/stat.h>
 
 /*************************************************************************************************
 ** Function:			main
 ** Description:		Uses getopt to run some system calls
 ** Precondition:		command line args -h, -t , or -f filename
 ** Postcondition:		n/a
 *************************************************************************************************/
int main(int argc, char *argv[]) {
	int option = 0;
	char c;
	struct utsname uname_pointer;
	time_t time_raw_format;
	struct stat s;
	
	while (( option = getopt(argc, argv, "htf:")) != -1) {
		switch (option) {
			case 'h':
				uname(&uname_pointer);
				printf("Hostname = %s \n", uname_pointer.nodename);
				break;
			case 't':
				time(&time_raw_format);
				printf("The current local time: %s", ctime(&time_raw_format));
				break;
			case 'f':
				if (stat(optarg, &s) == 0) {
					printf("size of file '%s' is %d bytes\n", optarg, (int) s.st_size);
				} else {
					printf("file '%s' not found\n", optarg);
				}
				break;
			default:
				return -1;
		}
	}
	return 0;
}
 
 