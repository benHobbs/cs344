#!/usr/bin/python

#Name:				Benjamin Hobbs
#Email:				hobbsbe@ondi.oregonstate.edu
#Class:					CS344-400
#Assignment:		Homework #5

#Description:		sets up primes from 100ktwins.txt for use in diff function

import csv
import re

file = "100ktwins.txt"

#lines = [ ]

#with open("100ktwins.txt") as f:
#    lines = f.read().splitlines()
	
#print lines

with open( file , 'r' ) as f:
    words_list =[]
    contents = open_file.readlines()
    for i in range(len(contents)):
        words_list.extend(contents[i].split())
		
print words_list
