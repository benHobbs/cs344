/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #2
 **************************************************************************************************
 ** Date Created:		7/6/2014
 ** File Name:			problem8.c
 ** Overview:			Test file for gbp debugger
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#define NUM_INTS 100

void func(int *ptr) {
	int i = 0;
	for (i = 0; i <= NUM_INTS; i++) {
		ptr[i] = i;
	}
}

int main (int argc, char *argv[]) {
	int *intArray = NULL;
	func(intArray);
	return 0;
}
