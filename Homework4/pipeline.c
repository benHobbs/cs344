/**************************************************************************************************
 ** Author:         			Benjamin Hobbs
 ** Email:						hobbsbe@onid.oregonstate.edu
 ** Class:						CS344-400
 ** Assignment:			Homework #4
 **************************************************************************************************
 ** Date Created:			8/3/2014
 ** File Name:				pipline.c
 ** Overview:				...
 ** Citations:				...
**************************************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main( int argc, char **argv )
{
	char *msg, *output_file;
	int option, number_pipes = 4, i = 0, dead;
	int pfd[number_pipes][2];
	int j = 0;
	//int test, status;
	//int pfd[num_pipes][2];
	pid_t childPid;
		msg = "pipeline- Correct Usage:\n\t\tpipeline -f [ output file ]\n";
	
	/*  Test to make sure enough arguments were provided, if not exit program  */
	if( argc != 3 ) {
		printf( "%s", msg );
		exit( -1 );
	}
	
	while (( option = getopt(argc, argv, "f:" )) != -1 ) {
		switch ( option ) {
			/**************************************************************************************
			 *	KEY 'f'
			 *	...
			 *************************************************************************************/
			case 'f':
				output_file = argv[ 2 ];
				//printf ( "%s : %d\n",output_file, parent);
				
				for ( i = 0 ; i <= number_pipes ; i++ ) {		/*	pg 516 */			
					if ( pipe ( pfd [ i ] ) == -1 ) {
						exit(	1 );
					}
					switch ( childPid = fork() ) {
						case -1:		//failure
							printf ( "pipeline - fork() failed" );
							exit ( -1 );
						case 0:			//child process created - child specific actions
							if ( i == 0 ) {	// initial, redirect stdout to pipe out
								if ( pfd [ i ] [ 1 ] != STDOUT_FILENO ) {
									if ( dup2 (pfd [ i ] [ 1 ] , STDOUT_FILENO) == -1 ) {
										exit ( -1 );
									}
								}
							} else if ( i == number_pipes ) {	//end, close all pipes
								if ( pfd [ i - 1 ] [ 0 ] != STDIN_FILENO ) {
									if ( dup2 ( pfd [ i - 1 ] [ 0 ] , STDIN_FILENO) == -1) {
										exit ( -1 );
									}
								}
							} else {		//printf ( "some other shit\n" );
								if ( pfd [ i ] [ 1 ] != STDOUT_FILENO ) {
									if ( dup2 (pfd [ i ] [ 1 ] , STDOUT_FILENO) == -1 ) {
										exit ( -1 );
									}
								}
								if ( pfd [ i - 1 ] [ 0 ] != STDIN_FILENO ) {
									if ( dup2 ( pfd [ i - 1 ] [ 0 ] , STDIN_FILENO) == -1) {
										exit ( -1 );
									}
								}
							}
							
							while (j <= i) {	//close open pipes
								if (close(pfd[j][0]) == -1) {
									exit(-1);
								}
								if (close(pfd[j][1]) == -1) {
									exit(-1);
								}
								j++;
							}
							
							switch ( i ) {
								case 0:
									execlp ( "rev" , "rev" , NULL );
								case 1:
									execlp( "sort" , "sort" , NULL );
								case 2:
									execlp( "uniq" , "uniq" , "-c" , NULL );
								case 3:
									execlp( "tee" , "tee" , output_file , NULL);
								case 4:
									execlp( "wc" , "wc" , NULL );
							}
							exit ( -1 );
						default:			//parent after successful fork
							//parent specific actions
							break;
						}
				}
				
				j = 0;
				while (j <= number_pipes) {	//close all pipes
					if (close(pfd[j][0]) == -1) {
									exit(-1);
								}
								if (close(pfd[j][1]) == -1) {
									exit(-1);
								}
								j++;
							}
				
				dead = 0;
				for (;;) { /* Parent waits for each child to exit */
					childPid = wait(NULL);
					if (childPid == -1) {
						if (errno == ECHILD) {	//no more children to cat
							//printf("No more children - bye!\n");
							exit ( 0 );
						} else { /* Some other (unexpected) error */
							exit ( -1 );
						}
					}
					dead++;
					//printf ( "wait() returned child PID %ld (numDead=%d)\n" , (long) childPid , dead );
					}
			default:
				break;
		}
	}
	sleep( 1 );	
	
	return 0;
}