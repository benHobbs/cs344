#!/usr/bin/python

#Name:				Benjamin Hobbs
#Email:				hobbsbe@ondi.oregonstate.edu
#Class:				CS344-400
#Assignment:		Homework #2
#Problem:			Problem 2

# citations:	http://www.cyberciti.biz/faq/python-command-line-arguments-argv-example/

import subprocess
import os
import sys
import getopt

#print "This is the name of the script: ", sys.argv[0]
#print "Number of arguments: ", len(sys.argv)
#print "The arguments are: " , str(sys.argv)

def main():

	path = "/nfs/stak/students/h/hobbsbe/classes/";
	term, course = "", ""

	if len(sys.argv) != 5:
		print ("Usage: %s -t term -c class OR %s --term term --class class" % (sys.argv[0], sys.argv[0]) )
		sys.exit(2)				#exit with prejudice

	try:
		opts, args = getopt.getopt(sys.argv[1:], "t:c:", ["term=","class="])
	except getopt.GetoptError as e:
		print (str(e))
		print ("Usage: %s -t term -c class OR %s --term term --class class" % (sys.argv[0], sys.argv[0]) )
		sys.exit(2)				#exit with prejudice

	for option, argument in opts:
		if option in ("-c", "--class"):
			course = argument
		elif option in ("-t", "--term"):
			term = argument

	if not os.path.exists( path):		#check if 'classes' dir exists
		os.mkdir( path, 0755 )

	path += term + "/";
	if not os.path.exists( path ):	#check if term exists
		os.mkdir( path, 0755 )		#if not create term directory

	path += course;
	if not os.path.exists( path ):	#check if course exists
		os.mkdir ( path, 0755 )		#if not create course and associated files
		os.mkdir ( path + "/assignments", 0755 )
		os.mkdir ( path + "/examples", 0755 )
		os.mkdir ( path + "/exams", 0755 )
		os.mkdir ( path + "/lecture_notes", 0755 )
		os.mkdir ( path + "/submissions", 0755 )
		
		dst = path + '/README'
		src = '/usr/local/classes/eecs/' + term + '/' + course + '/src/README'
		os.symlink(src, dst)
		
		dst = path + '/src_class'
		src = '/usr/local/classes/eecs/' + term + '/' + course + '/src'
		os.symlink(src, dst)

	else:
		print "Course '" + course + "' already exists."

if __name__ == '__main__':
	main()