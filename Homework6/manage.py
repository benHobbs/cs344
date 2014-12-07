#!/usr/bin/python
###############################################################################
##	Author:					Benjamin Hobbs
##	Email:					hobbsbe@onid.oregonstate.edu
##	Class:					CS344-400
##	Assignment:			Homework 6
###############################################################################
##	Date Created:		8/27/2014
##	File Name:			manage.py
##	Overview:			...
##	Citations:				http://www.binarytides.com/python-socket-server-code-example/
###############################################################################

import socket
import select
import string
import struct
import sys
import math

def usage():
	print "Usage: " + str(sys.argv[0]) + " port_# (must be same as report.py)";
	sys.exit()

if(len(sys.argv) > 1):
	try:
		port = int(sys.argv[1])
	except ValueError:
		usage()
else:
		usage()

con_lst = []
buf = 1024
port = int(sys.argv[1])
host = ''

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((host, port))
server_socket.listen(10)
 
con_lst.append(server_socket)	# Add server socket to the list of readable connections
 
while True:
	# Get the list sockets which are ready to be read through select
	read_sockets,write_sockets,error_sockets = select.select(con_lst,[],[])
		
	for current_socket in read_sockets:
		if current_socket == server_socket:		#new connection
			sockfd, addr = server_socket.accept()
			con_lst.append(sockfd)
			print "Client (%s, %s) connected" % addr
		else:		#Some incoming message from a client
			try:	# Data received from client, process it
				data = current_socket.recv(buf)
				if data:		# echo back the client message
					current_socket.send('OK ... ' + data)
			except:		# client disconnected, so remove from socket list
				broadcast_data(current_socket, "Client (%s, %s) is offline" % addr)
				print "Client (%s, %s) is offline" % addr
				current_socket.close()
				con_lst.remove(current_socket)
				continue
         
server_socket.close()