/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #5
 **************************************************************************************************
 ** Date Created:		8/12/2014
 ** File Name:			prime_pthread.c
 ** Overview:			...
 ** Citations:				...
**************************************************************************************************/
#define _POSIX_C_SOURCE 199309
#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

//bitmap
enum { BITS_PER_WORD = 8 }; 
unsigned char *bitmap;
#define WORD_OFFSET(b) ((b) / 8)
#define BIT_OFFSET(b)  ((b) % 8)
#define SHMEM_NAME "/hobbsbe"

unsigned int t_upper_bound;
unsigned int t_parallelism;
int verb_flag;

void setup_bitmap ( unsigned int bitmap_size );
void mark_non_prime ( unsigned int n );
void mark_prime ( unsigned int n );
float preload_bitmap ( unsigned int upper_bound );
float runner ( unsigned int parallelism , unsigned int upper_bound );
int build_threads ( void *( *func ) ( void * ) );
void *sync_threads ( void *n );
unsigned int get_next_prime ( unsigned int current_prime , unsigned int upper_bound );
float get_prime_count ( unsigned int upper_bound );
float print_twin_primes (unsigned int upper_bound );

 /*************************************************************************************************
 ** Function:				main
 ** Description:			...
 *************************************************************************************************/
int main (int argc, char **argv) {
	int option;
	int print_flag = 0;
	int c_flag = 0;
	int m_flag = 0;
	unsigned int upper_bound = 0;
	unsigned int parallelism = 0;
	unsigned int bitmap_size;
	float preload_time;
	float runner_time;
	float count_time;
	float print_time;
	
	t_upper_bound = 0;
	t_parallelism = 0;
	verb_flag = 0;
	
	while ( ( option = getopt ( argc , argv , "c:m:qvh" ) ) != -1 ) {
		switch ( option ) {
			case 'c':
				c_flag = 1;
				parallelism = atoi(optarg); //number of proc
				t_parallelism = atoi(optarg); //number of proc
				break;
			case 'm':
				m_flag = 1;
				upper_bound = atoi(optarg); //number entered
				t_upper_bound = atoi(optarg); //number entered
				break;
			case 'q':
				print_flag = 1;
				break;
			case 'v':				
				verb_flag = 1;
				break;
			case 'h':
				fprintf ( stderr , "Usage: ./pprimes -c [ parallelism ] -m [ upper boundry ] -q [ hide primes ] -v [ verbose ]\n" );
				exit ( -1 );
				break;
		}
	}

	//if flags are not set, set values to default
	if ( c_flag != 1 ) {
		parallelism = 1;
		t_parallelism = 1;
		//parallelism = 10;
	}
	
	if ( m_flag != 1 ) {
		upper_bound = UINT_MAX;
		t_upper_bound = UINT_MAX;
	}
	
	//setup bitmap
	if ( verb_flag == 1 )
		fprintf ( stderr , "Creating Bitmap:\n\tUpper boundy: %u\n" , upper_bound );
	bitmap_size = upper_bound / BITS_PER_WORD + 1;
	bitmap = malloc( bitmap_size * sizeof ( unsigned char ) );
	setup_bitmap ( bitmap_size );			//build bitmap (take care of case 1 and 2)
	fflush(stdout);
	
	//preload primes into bitmap
	if ( verb_flag == 1 )
		fprintf ( stderr , "Preloading Bitmap:\n\tParallelism: %u thread(s)\n" , parallelism );
	preload_time = preload_bitmap ( upper_bound );
	fflush(stdout);

	//run prime calculations with n processes
	if ( verb_flag == 1 )
		fprintf ( stderr , "Locating Primes:\n\tPlease Wait...\n" );
	runner_time = runner ( parallelism , upper_bound );
	if ( verb_flag == 1 )
		fprintf ( stderr , "Primes Located:\n\tTotal Time: %.2f seconds\n" , runner_time + preload_time);
	
	//count the primes
	if ( verb_flag == 1 )
		fprintf ( stderr , "Counting Primes:\n\tPlease Wait...\n" );
	count_time = get_prime_count ( upper_bound );
	if ( verb_flag == 1 )
		fprintf ( stderr , "Total Run Time: %.2f\n" , runner_time + preload_time + count_time );
	
	if ( print_flag != 1 ) {
		if ( verb_flag == 1 ) {
			fprintf ( stderr , "Printing:\n\tI would suggest redirection to a file for output...\n" );
			sleep ( 3 );
		}
		
		print_time = print_twin_primes ( upper_bound );
		if ( verb_flag == 1 )
			fprintf ( stderr , "Printing Complete:\n\tTime: %.2f\n" , print_time);
	}
	
	if( bitmap != NULL) {
		free ( bitmap );
	}
	
	return 0;
}

 /*************************************************************************************************
 ** Function:				setup_bitmap
 ** Description:			...
 *************************************************************************************************/
void setup_bitmap ( unsigned int bitmap_size ) {
	unsigned int i;
	
	for ( i = 0 ; i < bitmap_size ; i++ ) {
		bitmap [ i ] = 0b10101010;
	}
	
	//deal with 1 and 2
	mark_non_prime ( 1 );
	mark_prime ( 2 );
}

 /*************************************************************************************************
 ** Function:				mark_non_prime
 ** Description:			Mark bit 0 ( is not prime )
 *************************************************************************************************/
void mark_non_prime ( unsigned int n ) {
	bitmap [ WORD_OFFSET ( n ) ] &= ~ ( 1 << BIT_OFFSET ( n ) );
}


 /*************************************************************************************************
 ** Function:				mark_prime
 ** Description:			Mark bit 1 ( is prime )
 *************************************************************************************************/
void mark_prime ( unsigned int n ) {
	bitmap [ WORD_OFFSET ( n ) ] |= ( 1 << BIT_OFFSET ( n ) );
}

 /*************************************************************************************************
 ** Function:				preload_bitmap
 ** Description:			Using the Sieve of Eratosthenes to preload the bitmap.  Adapted
 **							from http://martin-thoma.com/generating-many-prime-numbers/
 *************************************************************************************************/
float preload_bitmap ( unsigned int upper_bound ) {
	unsigned int limit = sqrt ( upper_bound );
	unsigned int i;
	unsigned int j;
	time_t preload_start;
	time_t preload_end;
	
	time ( &preload_start );			//begin timing of prime preload
	
	for ( i = 3 ; i <= sqrt ( limit ) ; i++ ) {
		if ( ( bitmap [ WORD_OFFSET ( i ) ] & ( 1 << BIT_OFFSET ( i ) ) ) != 0 ) {
			for ( j = 3 ; ( i * j ) <= limit ; j++ ) {
				mark_non_prime (i * j);
			}
		}
	}
	
	time ( &preload_end );				//end timing of prime preload
	
	return difftime ( preload_end , preload_start );
}

 /*************************************************************************************************
 ** Function:				runner
 ** Description:			Runs the prime number calculation using /parallelism/ processes
 *************************************************************************************************/
float runner ( unsigned int parallelism , unsigned int upper_bound ) {
	time_t runner_start;
	time_t runner_end;
	int x;
	
	time ( &runner_start );				//begin timing of prime calculation

	x = build_threads( &sync_threads );
	
	time ( &runner_end );				//end timing of prime calculation

	return difftime ( runner_end , runner_start );
}

 /*************************************************************************************************
 ** Function:				build_threads
 ** Description:			Uses function sync_threads to build parallelism threads
 *************************************************************************************************/
int build_threads ( void *( *func ) ( void * ) ) {
	unsigned int i;

	pthread_t *thread;

	thread = malloc( t_parallelism * sizeof ( pthread_t ) );

	for ( i = 0 ; i < t_parallelism ; i++ ) {
		if (pthread_create ( &thread[ i ] , NULL , func , ( void * ) ( intptr_t ) i ) != 0 ) {
			fprintf ( stderr , "./prime_mproc:  pthread_create ( ) failed\n" );
			exit ( -1 );
		}
	}

	for ( i = 0 ; i < t_parallelism ; i++ ) {
		pthread_join( thread [ i ] , NULL );
	}
	
	return 0;
}

 /*************************************************************************************************
 ** Function:				sync_threads
 ** Description:			...
 *************************************************************************************************/
void *sync_threads ( void *n ) {
	//get current location
	unsigned int curr = ( unsigned int ) ( intptr_t ) n;
	//set min for current thread
	unsigned int min = curr * (t_upper_bound / t_parallelism) + 1;
	//set max for current thread
	unsigned int max = (curr == t_parallelism - 1) ? t_upper_bound : min + (t_upper_bound / t_parallelism) - 1;
	unsigned int i;
	unsigned long j;
	
	i = 1;
	while ( ( i = get_next_prime ( i , t_upper_bound ) ) != 0 ) {
		for ( j = ( min / i < 3 ) ? 3 : ( min / i ) ; ( i * j ) <= max ; j++ ) {
			mark_non_prime ( i * j );
		}
	}
	
	pthread_exit(EXIT_SUCCESS);
}

 /*************************************************************************************************
 ** Function:				get_next_prime
 ** Description:			Returns the next prime in the bitmap
 *************************************************************************************************/
unsigned int get_next_prime ( unsigned int current_prime , unsigned int upper_bound ) {
	unsigned int i;
	
	for ( i = current_prime + 1 ; i <= sqrt ( upper_bound ) ; i++ ) {
		if ( ( bitmap [ WORD_OFFSET ( i ) ] & ( 1 << BIT_OFFSET ( i ) ) ) != 0 ) {
			return i;
		}
	}
	
	return 0;
}

 /*************************************************************************************************
 ** Function:				get_prime_count
 ** Description:			Counts the primes, times itself, and then prints both of those vals
 **							Also used to print twin primes if q key is not used
 *************************************************************************************************/
float get_prime_count ( unsigned int upper_bound ) {
	unsigned int i;
	unsigned int j;
	unsigned int prev = 0;
	unsigned int curr = -1;
	unsigned int prime_count = 0;
	unsigned int twin_prime_count = 0;
	time_t count_start;
	time_t count_end;

	time ( &count_start );					//begin timing of prime count
	
	for ( i = 0 ; i < ( upper_bound / BITS_PER_WORD + 1 ) ; i++ ) {
		if ( bitmap [ i ] == 0 ) {
			continue;
		}
		
		for ( j = 0 ; j < 8 ; j ++ ) {
			if ( ( bitmap [ WORD_OFFSET ( ( 8 * i ) + j ) ] & ( 1 << BIT_OFFSET ( ( 8 * i ) + j ) ) ) != 0 ) {
				prime_count++;
				
				prev = curr;
				curr = j;
				if ( curr - prev == 2 ) {
					twin_prime_count++;
				}
			}
		}
	}
	
	time ( &count_end );				//end timing of prime count
	
	if ( verb_flag == 1 )
		fprintf ( stderr , "Primes Counted:\n\tNumber of Primes: %u\n\tNumber of Twin Primes: %u\n\tTime: %.2f seconds\n" , prime_count , twin_prime_count , difftime ( count_end , count_start ) );
	
	return difftime ( count_end , count_start );
}


 /*************************************************************************************************
 ** Function:				print_twin_primes
 ** Description:			...
 *************************************************************************************************/
float print_twin_primes (unsigned int upper_bound ) {
	unsigned int i;
	unsigned int prev = 0;
	unsigned int curr = -1;
	time_t count_start;
	time_t count_end;
	
	time ( &count_start );					//begin timing of prime count

	for (i = 2 ; i <= upper_bound ; i++) {
		if ( ( bitmap [ WORD_OFFSET ( i ) ] & ( 1 << BIT_OFFSET ( i ) ) ) != 0 ) {
			prev = curr;
			curr = i;
			if ( curr - prev == 2 ) {
				fprintf( stdout , "%u %u\n" , prev , curr );
				fflush( stdout );
			}
		}
	}
	
	time ( &count_end );				//end timing of prime count
	
	return difftime ( count_end , count_start );
}