 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <ctype.h>
 #include <math.h>


/*
	Needed constants for working with buffers, streams etc.

*/
#ifndef BUF_SIZE
#define BUF_SIZE 512
#endif 

#define MAX_ENTROPY 20 


/*
	UTILITY FUNCTION PROTOTYPES
*/

 

/*
	Input formatter in the valid form [0-9 A-Z a-z].
	Receives a string with possibly invalid characters ['$', '@', '\tab', etc..] and purifies it to the form [0-9A-Za-z].

	Args:	< *inp >:	pointer to original string.
			< *out >:	pointer to output.
	Returns: NULL if inp is NULL, the purified string in any other case.

*/
char *formatInput(char *inp, char *out);

/*
	Entropy collector.Creates a pseudo-random encryption key by utilising /dev/urandom.
	
	Args:	<num>:	Number of bits to be generated  [terminating character '\0' is automatically appended] and should not be counter in <num>.
	Returns: The pseudo-random key in char form.
*/
char *createKey(unsigned int num);


char *readInput();


