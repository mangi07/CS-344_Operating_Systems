# Oregon State University CS 344 Operating Systems
# Assignment 5: Python Exploration
# Author: Ben R. Olson
# Date: 5/23/2016

import random
import string

# This method was borrowed from: http://stackoverflow.com/questions/2823316/generate-a-random-letter-in-python
# Returns string of y number of lowercase letters
def random_char(y):
	return ''.join( random.choice( string.ascii_lowercase ) for x in range( y ) )

# Write 10 random lowercase letters to each of three files
files = [ 'file_one', 'file_two', 'file_three' ]
for f in files:
	try:
		curr_f = open( f, 'w' )
	except IOERRor:
		print "Could not open one or more files."

	s = random_char( 10 )
	curr_f.write( s )
	curr_f.write( '\n' );

	curr_f.close()
	print "\"%s\" was written to file named \"%s\"" % ( s, f )

# Print out two random integers (range 1 to 42, inclusive)
number_one = random.randrange( 1, 43 )
number_two = random.randrange( 1, 43 )
print "Random integer one: %d" % ( number_one )
print "Random integer two: %d" % ( number_two )

# Print out the product of these two integers
print "Product of these two numbers: %d" % ( number_one * number_two )

