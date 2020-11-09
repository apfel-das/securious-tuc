#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>


/*
 * Prints the hex value of the input, 16 values per line
 *
 * arg0: data
 * arg1: data len
 */
void
print_hex(unsigned char *, size_t);


/*
 * Prints the input as string
 *
 * arg0: data
 * arg1: data len
 */
void print_string(unsigned char *, size_t);


/*
 * Prints the usage message
 */
void usage(void);


/*
 * Checks the validity of the arguments
 *
 * arg0: path to input file
 * arg1: path to output file
 * arg2: path to key file
 * arg3: operation mode
 */
void check_args(char *, char *, char *, int);


/*

	Reads a file's content.

	arg1: path to input file.
	arg2: file length.
	Note: <arg2> will be calculated inside function, therefore it is suggested to be passed as NULL.


*/

unsigned char *readFile(char *fPath, unsigned long *len);


/*
	Writes content on a file.

	arg1: path to output file.
	arg2: pointer on content to be written as an <unsigned char *>.
	arg3: length of content.
	
	Note: <arg3> should not be NULL, it is suggested to be strlen(const char *arg3) or something similar for non-'\0' terminated content.


*/

void writeFile(char *fPath, unsigned char *data, unsigned long len);


#endif /* _UTILS_H */


