#!/usr/bin/python

#Name:				Benjamin Hobbs
#Email:				hobbsbe@ondi.oregonstate.edu
#Class:				CS344-400
#Assignment:		Homework #2
#Problem	:			Problem 5

# citations:			http://www.pythonforbeginners.com/os/subprocess-for-system-administrators

import subprocess
import os
import sys

p = subprocess.Popen(['who'],stdout=subprocess.PIPE)
stdout, stdin = p.communicate()
print stdout