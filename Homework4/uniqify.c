/**************************************************************************************************
 ** Author:         			Benjamin Hobbs
 ** Email:						hobbsbe@onid.oregonstate.edu
 ** Class:						CS344-400
 ** Assignment:			Homework #4
 **************************************************************************************************
 ** Date Created:			8/3/2014
 ** File Name:				uniqify.c
 ** Overview:				...
 ** Citations:				...
**************************************************************************************************/

#define _POSIX_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int parse_args ( int argc , char **argv );
void child_sort ( int child_number , int number_pipes , int pfd_0 [ ] [ 2 ] , int pfd_1 [ ] [ 2 ] ); 
void merge ( int number_pipes , FILE* stream [ ] , int pfd_0 [ ] [ 2 ] , int pfd_1 [ ] [ 2 ] );
void parse ( int number_pipes, FILE* parser_to_sort_stream[] , int arr0[][2], int arr1[][2] );


int main( int argc , char **argv )
{
	int i;
	int number_pipes = parse_args( argc , argv );
	int pfd_0 [ number_pipes ] [ 2 ];
	int pfd_1 [ number_pipes ] [ 2 ];
	
	FILE *parse_sort_stream [ number_pipes ];
	FILE *sort_merge_stream [ number_pipes ];
	pid_t childPid[number_pipes];
	
	//printf ( "%d\n" , number_pipes );
	
	//build some pipes
	for ( i = 0 ; i < number_pipes ; i++ ) {
		if ( pipe ( pfd_0 [ i ] ) == -1 ) {
			printf ( "uniqify - build pipe failed" );
			exit ( -1 );
		}
		if ( pipe ( pfd_1 [ i ] ) == -1 ) {
			printf ( "uniqify - build pipe failed" );
			exit ( -1 );
		}
	}
	
	//fork off the sorting processes
	for ( i = 0 ; i < number_pipes ; i++ ) {
		switch ( childPid [ i ] = fork() ) {
			case -1:		//failure
				printf ( "uniqify - fork() failed" );
				exit ( -1 );
			case 0:			//child process created - child specific actions
				child_sort ( i , number_pipes , pfd_0 , pfd_1 );
			default:
				break;
		}
	}
	
	//fork off merging process
	switch ( fork() ) {
		case -1:		//failure
			printf ( "uniqify - fork() failed" );
			exit ( -1 );
		case 0:			//child process created - child specific actions
			//parse ( number_pipes , parse_sort_stream , pfd_0 , pfd_1 );
			
			merge ( number_pipes , sort_merge_stream , pfd_0 , pfd_1 );
		default:
			//merge ( number_pipes , sort_merge_stream , pfd_0 , pfd_1 );
		
			parse ( number_pipes , parse_sort_stream , pfd_0 , pfd_1 );
			//break;
	}
	
	while(wait(NULL) != -1);
	
	return 0;
}

int parse_args ( int argc , char **argv ) {
	char *msg;
	int option;
	int val;
		msg = "uniqify - Correct Usage:\n\t\tuniqify -n [ number of process for sort function (1 - 50) ]\n";
	
	if( argc != 3 ) {
		fprintf( stderr , "%s", msg );
		exit( -1 );
	}
	
	while (( option = getopt(argc, argv, "n" )) != -1 ) {
		switch ( option ) {
			case 'n':
				val = atoi ( argv [ 2 ] );
				
				if ( val < 1 || val > 50 ) {
					fprintf( stderr , "%s", msg );
					exit ( -1 );
				}
				return val;
			default:
				break;
		}
	}
	return 1;
}

void child_sort ( int child_number, int number_pipes, int pfd_0 [ ] [ 2 ], int pfd_1 [ ] [ 2 ] ) {
	int i;
	//int buf;
	//char char_buf[ 100 ];

	//close n -1 read pipes, and n-1 write pipes
    for ( i = 0 ; i < number_pipes ; i++ ) {
        if ( i != child_number ) {
            if ( close ( pfd_0 [ i ] [ 0 ] ) == -1 ) {
				fprintf (stderr , "uniqify - closing reader pipe %d on child %d failed\n" , i , child_number );
				exit ( -1 );
            }
            if ( close ( pfd_1 [ i ] [ 1 ] ) == -1 ) {
                fprintf (stderr , "uniqify - closing write pipe %d on child %d failed\n" , i , child_number );
				exit ( -1 );
            }
        }
    }
	
	//close all parser to sort writes and all sort to merger reads
	for ( i = 0 ; i < number_pipes ; i++ ) {
        if ( close ( pfd_1 [ i ] [ 0 ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing reader pipe %d on child %d failed - 2\n" , i , child_number );
			exit ( -1 );
        }
        if ( close ( pfd_0 [ i ] [ 1 ] ) == -1 ) {
            fprintf (stderr , "uniqify - closing write pipe %d on child %d failed - 2\n" , i , child_number );
			exit ( -1 );
        }
    }

	//redirect input and ouput
	if ( dup2 ( pfd_0 [ child_number ] [ 0 ] , STDIN_FILENO ) == -1 ) {		//read
		fprintf (stderr , "uniqify - Redirect of STDIN on child %d failed\n" , child_number );
		exit ( -1 );
	}
	
	if ( dup2 ( pfd_1 [ child_number ] [ 1 ]  , STDOUT_FILENO ) == -1 ) {	//write
		fprintf (stderr , "uniqify - Redirect of STDOUT on child %d failed\n" , child_number );
		exit ( -1 );
	}
	
	//sort
	//execlp("sort", "sort", NULL);
	
	//close last two pipes
	if ( close ( pfd_0 [ child_number ] [ 0 ] ) == -1 ) {
		fprintf (stderr , "uniqify - closing final read pipe on child %d failed\n" , child_number );
		exit ( -1 );
    }
    if ( close ( pfd_1 [ child_number ] [ 1 ] ) == -1 ) {
        fprintf (stderr , "uniqify - closing final read pipe on child %d failed\n" , child_number );
		exit ( -1 );
    }
	
	
	execlp("sort", "begSort", NULL);
	
	exit ( EXIT_SUCCESS );
}

void merge ( int number_pipes , FILE* stream [ ] , int pfd_0 [ ] [ 2 ] , int pfd_1 [ ] [ 2 ] ) {
	char buf [ number_pipes ] [ 100 ];
	char tmp_buf [ 100 ];
	char min_buf [ 100 ];
	char null_str [ ] = "\0";
	char z_str [ ] = "zzzzzzzzzz\0";
	int min_index;
	int i;
	int count;
	int read_pipes = 0;
	
	for ( i = 0 ; i < number_pipes ; i++ ) {
		if ( close ( pfd_1 [ i ] [ 1 ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing write pipe %d, in merger, failed\n" , i );
			exit ( -1 );
		}
		if ( close ( pfd_0 [ i ] [ 1 ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing parser to sort write pipe %d, in merger, failed\n" , i );
			exit ( -1 );
		}
		
		//->
		if ( close ( pfd_0 [ i ] [ 0 ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing parser to sort read pipe %d, in merger, failed\n" , i );
			exit ( -1 );
		}
		
		//open stream i for read
		stream [ i ] = fdopen ( pfd_1 [ i ] [ 0 ] , "r" );
	}
	
	count = 0;
	min_index = -1;
	
	//preload pipes
	for ( i = 0 ; i < number_pipes ; i++ ) {			
		if ( fgets ( buf [ i ] , 99 , stream [ i ] ) != NULL ) {
		} else {
			strcpy ( buf [ i ] , null_str );	//if blank, make null
			read_pipes++;
		}
	}	
	
	while ( read_pipes < number_pipes ) {
		//get min value
		strcpy ( min_buf , z_str );
		min_index = 0;
		for ( i = 0 ; i < number_pipes ; i++ ) {
			if ( strcmp ( min_buf , buf [ i ] ) > 0 ) {	//min_buf > buf [ i ]
				strcpy ( min_buf , buf [ i ] );
				min_index = i;
			}
		}
		
		if ( fgets ( buf [ min_index ] , 99 , stream [ min_index ] ) != NULL ) {	//reset i
		
		} else {
			strcpy ( buf [ min_index ] , null_str );	//if blank, make null
			read_pipes++;
		}
		
		//fprintf ( stdout, "%d\t%s" , count , min_buf );	//actually print tmp
		//count/print
		if ( count == 0 ) {	//first run through
			strcpy ( tmp_buf, min_buf );
			count = 1;
		}
		
		if ( strcmp( tmp_buf , min_buf ) != 0 ) {
			
			fprintf ( stdout, "%7d %s" , count , tmp_buf );	//actually print tmp
			strcpy ( tmp_buf, min_buf );
			count = 1;
		} else {
			count++;
		}
	}
	
	//close read streams
	for ( i = 0 ; i < number_pipes ; i++ ) {
		if ( fclose ( stream [ i ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing stream %d, in merger, failed\n" , i );	
			exit ( -1 );
		}
	}
	
	exit ( EXIT_SUCCESS );
}


void parse ( int number_pipes, FILE* stream[] , int pfd_0 [ ] [ 2 ] , int pfd_1 [ ] [ 2 ] ) {
    char read_buffer [ 100 ];
	char tmp_buffer [ 100 ];
    int buffer_len;
    int i;
    int j;
	
    /* close all read end(s) of pipe for parent*/
    for( i = 0 ; i < number_pipes ; i++ ) {
		if ( close ( pfd_0 [ i ] [ 0 ] ) == -1) {
			fprintf (stderr , "uniqify - closing parser to sort read pipe %d, in parse process, failed\n" , i );
			exit( - 1 );
		}
		if ( close ( pfd_1 [ i ] [ 0 ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing sort to parser read pipe %d, in parse process, failed\n" , i );
            exit( - 1 );
        }
        if ( close ( pfd_1 [ i ] [ 1 ] ) == -1 ) {
			fprintf (stderr , "uniqify - closing sort to parser write pipe %d, in parse process, failed\n" , i );
            exit( - 1 );
        }
        stream[i] = fdopen (pfd_0 [ i ] [ 1 ] , "w" );
    }

    /* write string to children through n pipes in round robin fashion*/
    for ( i = 0 ; fscanf ( stdin , " %99[a-zA-Z]" , read_buffer ) != EOF ; i++ ) {
		if ( fscanf ( stdin , " %99[^a-zA-Z]", tmp_buffer ) != 1 ) {	//clear buffer
			
		}
		/* make entire string lower case*/
		buffer_len = strlen ( read_buffer );
		for ( j = 0 ; j < buffer_len ; j++ ) {
			read_buffer [ j ] = tolower ( read_buffer [ j ] );
		}
		fputs ( read_buffer , stream [ ( i + number_pipes ) % number_pipes ] );
		fputs ( "\n" , stream [ ( i + number_pipes ) % number_pipes ] );
		//fprintf(stderr, "%s (size = %d) (stream = %d)\n", read_buffer, buff_len, ((i + number_pipes)%number_pipes));
    }

    /* parent clean-up, send EOF to child by closing stream*/
    for(i = 0; i < number_pipes; i++){
        fclose(stream[i]);
    }
	
	exit ( EXIT_SUCCESS );
}