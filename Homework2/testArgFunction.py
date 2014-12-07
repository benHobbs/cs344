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

def args(usagelist):
	msg =  "Usage: " + str(sys.argv[ 0 ]) + " " + usagelist[ 0 ] + " OR " + str(sys.argv[ 0 ]) + " " + usagelist[ 1 ]
	
	if len(sys.argv) != 3:
		print msg
		sys.exit(2)				#exit with prejudice

	try:
		opts, args = getopt.getopt( sys.argv[1:], usagelist[2], [ usagelist[3] ] )
	except getopt.GetoptError as e:
		print (str(e))
		print msg
		sys.exit(2)				#exit with prejudice

	for option, argument in opts:
		if option in ( "-n", "--nprime" ):
			n = argument
	
	return n
	
###################################################################################################
## Start Script
###################################################################################################

#argslist[n]
usagelist = ['-n nth_prime','--nprime nth_prime','n:', 'nprime=']



n = args(usagelist)

