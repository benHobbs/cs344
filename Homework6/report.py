#!/usr/bin/python
###############################################################################
##	Author:					Benjamin Hobbs
##	Email:					hobbsbe@onid.oregonstate.edu
##	Class:					CS344-400
##	Assignment:			Homework 6
###############################################################################
##	Date Created:		8/27/2014
##	File Name:			report.py
##	Overview:			...
##	Citations:				...
###############################################################################

import socket
import string
import sys

def usage():
	print "Usage: " + str(sys.argv[0]) + " port_# (must be same as report.py)";
	sys.exit()

if(len(sys.argv) == 3 and str(sys.argv[2]) == "-k"):
	try:
		kill = 1
		port = int(sys.argv[1])
	except ValueError:
		usage()
elif(len(sys.argv) == 2):
	try:
		port = int(sys.argv[1])
	except ValueError:
		usage()
else:
		usage()

s = socket.socket()         # Create a socket object
s.connect(('', port))
print s.recv(1024)

s.close                     # Close the socket when done

#if kill:
#	conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#	conn.connect((host,port))
#	print "terminating servers and client"
#	conn.sendall(chr(35))	#send terminate notice
#	conn.shutdown(1)
#	conn.close()