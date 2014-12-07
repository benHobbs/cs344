#!/usr/bin/python

#Name:				Benjamin Hobbs
#Email:				hobbsbe@ondi.oregonstate.edu
#Class:				CS344-400
#Assignment:		Homework #2
#Problem	:			Problem 3

# citations:			http://www.cyberciti.biz/faq/python-command-line-arguments-argv-example/
#							http://www.pythonforbeginners.com/python-on-the-web/how-to-use-urllib2-in-python/

import os
import sys
import getopt
import urllib2

def main():

	url, file = "", ""
	msg = "Usage: " + str(sys.argv[0]) + " -u url -f file OR " + str(sys.argv[0]) + " --url url --file file"
	
	if len(sys.argv) != 5:
		print msg
		sys.exit(2)				#exit with prejudice

	try:
		opts, args = getopt.getopt(sys.argv[1:], "u:f:", ["url=", "file="])
	except getopt.GetoptError as e:
		print (str(e))
		print msg
		sys.exit(2)				#exit with prejudice

	for option, argument in opts:
		if option in ("-u", "--url"):
			url = argument
		if option in ("-f", "--file"):
			file = argument
	
	if url[:7] != "http://":		#make sure appropriate protocol
		url = "http://"+url
		
	try:
		response = urllib2.urlopen(url)
	except URLError, e:
		print e.reason
		sys.exit(2)			#exit with prejudice
	
	test = open(file, "w")
	test.write(response.read())
	test.close()
	
	print("File %s:  Successfully Downloaded From %s." % ( file, url ) )
	
if __name__ == '__main__':
	main()