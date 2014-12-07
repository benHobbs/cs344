/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #5
 **************************************************************************************************
 ** Date Created:		8/12/2014
 ** File Name:			prime_mproc.c
 ** Overview:			...
 ** Citations:				...
**************************************************************************************************/
#define _POSIX_C_SOURCE 199309
#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <ar.h>
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

int verb_flag;

void *setup_shared_memory ( char *path , int shmem_size );
void setup_bitmap ( unsigned int bitmap_size );
void mark_non_prime ( unsigned int n );
void mark_prime ( unsigned int n );
float preload_bitmap ( unsigned int upper_bound );
float runner ( unsigned int parallelism , unsigned int upper_bound , pid_t *pid_array );
unsigned int get_next_prime ( unsigned int current_prime , unsigned int upper_bound );
float get_prime_count ( unsigned int upper_bound );
float print_twin_primes (unsigned int upper_bound );

 /*************************************************************************************************
 ** Function:				main
 ** Description:			...
 *************************************************************************************************/
int main( int argc , char **argv ) {
	int option;
	int print_flag = 0;
	int c_flag = 0;
	int m_flag = 0;
	unsigned int upper_bound = 0;
	unsigned int parallelism = 0;
	unsigned int bitmap_size;
	void *addr;
	float preload_time;
	float runner_time;
	float count_time;
	float print_time;
	pid_t *pid_array;
	verb_flag = 0;

	while ( ( option = getopt ( argc , argv , "c:m:qvh" ) ) != -1 ) {
		switch ( option ) {
			case 'c':
				c_flag = 1;
				parallelism = atoi(optarg); //number of proc
				break;
			case 'm':
				m_flag = 1;
				upper_bound = atoi(optarg); //number entered
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
		//parallelism = 10;
	}
	
	if ( m_flag != 1 ) {
		upper_bound = UINT_MAX;
	}
	
	//setup bitmap
	if ( verb_flag == 1 )
		fprintf ( stderr , "Creating Bitmap:\n\tUpper boundy: %u\n" , upper_bound );
	bitmap_size = upper_bound / BITS_PER_WORD + 1;
	addr = setup_shared_memory ( SHMEM_NAME , bitmap_size );
	bitmap = (unsigned char *) addr;		//init bitmap
	setup_bitmap ( bitmap_size );			//build bitmap (take care of case 1 and 2)
	fflush(stdout);
	
	//preload primes into bitmap
	if ( verb_flag == 1 )
		fprintf ( stderr , "Preloading Bitmap:\n\tParallelism: %u process(es)\n" , parallelism );
	preload_time = preload_bitmap ( upper_bound );
	fflush(stdout);

	//run prime calculations with n processes
	if ( verb_flag == 1 )
		fprintf ( stderr , "Locating Primes:\n\tPlease Wait...\n" );
	pid_array = malloc( sizeof ( pid_t ) * parallelism );	//setup array for child process ids
	runner_time = runner ( parallelism , upper_bound , pid_array );
	if ( verb_flag == 1 )
		fprintf ( stderr , "Primes Located:\n\tTotal Time: %.2f seconds\n" , runner_time + preload_time);
	
	//count the primes
	if ( verb_flag == 1 )
		fprintf ( stderr , "Counting Primes:\n\tPlease Wait...\n" );
	count_time = get_prime_count ( upper_bound );
	//count_time += get_twin_prime_count ( upper_bound , 1 );
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

	
	if ( shm_unlink ( SHMEM_NAME ) == -1) { //Delete shared memory object
		printf( "./prime_mproc:  Deleting shared memory object failed.\n" );
		exit( -1 );
	}

	free ( pid_array );
	
	return 0;
}

 /*************************************************************************************************
 ** Function:				setup_shared_memory
 ** Description:			Lifted from TLPI, page 1112
 *************************************************************************************************/
void *setup_shared_memory ( char *path , int shmem_size ) {
	void *addr;
	int fd = shm_open ( path , O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
	
	if ( fd == -1 ) {
		fprintf ( stderr , "./prime_mproc:  Opening shared memory object failed.\nPath: %s\n" , path );
		exit ( -1 );
	}

	if ( ftruncate ( fd , shmem_size ) == -1 ) {
		fprintf ( stderr , "./prime_mproc:  Truncating shared memory object failed.\n" );
		exit( -1 );
	}

	//map shared memory object
	addr = mmap ( NULL , shmem_size , PROT_READ | PROT_WRITE, MAP_SHARED, fd , 0 );
	
	if ( addr == MAP_FAILED ) {
		fprintf ( stderr , "./prime_mproc:  Opening shared memory object failed.\nPath: %s\n" , path );
		exit ( -1 );
	}
	
	return addr;
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
float runner ( unsigned int parallelism , unsigned int upper_bound , pid_t *pid_array ) {
	unsigned int i;
	unsigned int j;
	unsigned long k;
	unsigned long l;
	unsigned int min;
	unsigned int max;  
	time_t runner_start;
	time_t runner_end;
	
	time ( &runner_start );				//begin timing of prime calculation

	for ( i = 0 ; i < parallelism ; i++ ) {
		switch ( pid_array [ i ] = fork ( ) ) {
			case -1:
				fprintf ( stderr , "./prime_mproc:  fork ( %u ) failed.\n" , i );
				exit ( -1 );
			case 0:
				min = i * (upper_bound / parallelism ) + 1;
				if( i == parallelism -1)
					max = upper_bound;
				else
				max = min + ( upper_bound / parallelism ) - 1;

				j = 1;
				while ( ( j = get_next_prime ( j , upper_bound ) ) != 0 ) {
					if ( ( min / j ) < 3 )
						l = 3;
					else	
						l = min / j;
					for ( k = l ; ( j * k ) <= max ; k++ ) {
						mark_non_prime ( j * k );
					}
				}
				exit ( 0 );
			default:
				break;
			}
	}
	
	for ( i = 0; i < parallelism ; i++ ) {	//wait on children
		wait( NULL );
	}
	
	time ( &runner_end );				//end timing of prime calculation

	return difftime ( runner_end , runner_start );
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