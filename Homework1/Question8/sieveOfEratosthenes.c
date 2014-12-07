/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #1
 **************************************************************************************************
 ** Date Created:		6/27/2014
 ** File Name:			seiveOfEratosthenes.c
 ** Overview:			Program is the driver for a function that runs the sieve of 
 **							Eratosthenes for n primes, as input by the user.
**************************************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
/*************************************************************************************************
 ** Function:			soe
 ** Description:		Algorithm for the sieve of Eratosthenes.
 ** Precondition:		int n:				upper bound
 **							int **primes:	array for primes
 ** Postcondition:		returns int (number of primes)
 **							int **primes will contain all primes under upper bound
 *************************************************************************************************/
int soe(int **primes, int n) {
	int k, primeCount, ArrayOfInts[n], i, primeIndex;
	
	for(i = 0 ; i <= n ; i++) {	//fill the int array with markers
		ArrayOfInts[i] = 1;
	}
	
	k = 1;							//set k equal to 1
	ArrayOfInts[k] = -1;		//mark 1 as special
	
	while(k <= (sqrt(n))) {	//loops through k values until square root n
		k++;	//increment k
		if(ArrayOfInts[k] == 1) {	//find the first number greater than k that
												//has not been id'd as composite
			int m = k;
			int compositeCount = 2;
			while((m * compositeCount) < n) {
				ArrayOfInts[m * compositeCount] = 0;
				compositeCount++;
			}
		}
	}
	
	primeCount = 0;					//set primeCount to 0
	for(i = 1; i < n; i++) {				//get number of primes
		if(ArrayOfInts[i] == 1) {
			primeCount++;
		}
	}
	
	*primes = (int *)malloc(sizeof(int) *primeCount);
	primeIndex = 0;
	
	for(i = 1; i < n; i++) {
		if(ArrayOfInts[i] == 1) {
			*(*primes + primeIndex) = i;
			primeIndex++;
		}
	}
	
	return primeCount;
}
 
 
 /*************************************************************************************************
 ** Function:				main
 ** Description:		Driver for homework assignment.
 ** Precondition:		n/a
 ** Postcondition:		n/a
 *************************************************************************************************/
int main() {
 
	int *primes, **primePointer = &primes, n, i;
	
	printf("\nWe are going to calculate the first n primes using the Sieve Of Eratosthenes.\n");
	printf("Please enter your desired upper boundary:  ");
	scanf("%d", &n);	
	
	int primeCount = soe(primePointer,  n);
	
	printf("\nThere are %d primes between 1 and %d.  They are:\n", primeCount, n);
	for( i=0 ; i < primeCount ; i++ ) {
		printf("%d ", *(primes + i));
	}
	printf("\n");
 
	return 0;
}
 
 