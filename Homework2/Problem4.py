#!/usr/bin/python

#Name:				Benjamin Hobbs
#Email:				hobbsbe@ondi.oregonstate.edu
#Class:				CS344-400
#Assignment:		Homework #2
#Problem	:			Problem 4

# citations:			http://www.cyberciti.biz/faq/python-command-line-arguments-argv-example/
#							http://www.pythonforbeginners.com/python-on-the-web/how-to-use-urllib2-in-python/

import os
import sys
import getopt
import urllib2
import math

def args():
	msg =  "Usage: " + str(sys.argv[0]) + " -n nth_prime OR " + str(sys.argv[0]) + " --nprime nth_prime"
	
	if len(sys.argv) != 3:
		print msg
		sys.exit(2)				#exit with prejudice

	try:
		opts, args = getopt.getopt( sys.argv[1:], "n:", ["nprime="] )
	except getopt.GetoptError as e:
		print (str(e))
		print msg
		sys.exit(2)				#exit with prejudice

	for option, argument in opts:
		if option in ( "-n", "--nprime" ):
			n = argument
	
	return n
	
def is_prime( num ):
	for i in range( 2, int( math.sqrt( num ) + 1 ) ):
		if num % i == 0:
			return False
	return True
	
def get_prime( n ):
	primecount, num = 0, 2
	while primecount < n:
		if is_prime( num ):
			primecount += 1
			if primecount == n:
				return num
		num += 1
	
	
###################################################################################################
## Start Script
###################################################################################################

n = args()
prime = get_prime( int( n ) )
print( "The %d prime number is: %d" % ( int(n), int(prime) ) )
