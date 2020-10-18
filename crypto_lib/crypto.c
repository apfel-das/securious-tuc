#include "crypto.h"



int main()
{

	char *cipher = NULL;
	uint8_t *key = NULL;
	char *enc = NULL;
	char *orig = NULL;

	printf("Hello crypto-curious..\n");
	

	printf("plaintext: ");
	cipher = readInput();
	cipher = formatInput(cipher, enc);
	
   	key = getRandomKey(strlen(cipher));

	

	return 0;
}


/*
	
	** OPERATION HANDLERS **

*/

/*
	Given a character, returns the respective index in charPool.
	Args:		The character to be tracked.
	Returns:	The respective index in the adjacent matrix or -1 if not found.
*/
int getPoolIndex(char c)
{
	//raw parsing is just a fun way of deciding.
	for (int i = 0; i < POOL_SIZE; i++)
	{	
		//if character given is present [A-Z, a-z, 0-9], return the index in the adjacent matrix.
		if(c == *(charPool + i))
			return i;
	}

	//return failure.
	return -1;
}

/*
	Given a character, returns the respective index in caps.
	Args:		The character to be tracked.
	Returns:	The respective index in the adjacent matrix or -1 if not found.
*/
int getCapsIndex(char c)
{
	//raw parsing is just a fun way of deciding.
	for (int i = 0; i < ALPHA_POOL_SIZE; i++)
	{	
		//if character given is present [A-Z], return the index in the adjacent matrix.
		if(c == *(caps + i))
			return i;
	}

	//return failure.
	return -1;
}

/*
	Return the length of the converted string in terms of 8-byte integers.
*/

int getLength(uint8_t *arg1)
{
	return (sizeof((char *)arg1) / sizeof(uint8_t));
}


/*
	Returns a random alpha-arithmetic digit using a file descriptor on "/dev/urandom".
	More: "crypto.h"
*/
char getRandomDigit()
{

	//open the file descriptor "showing" the entropy-library.
	randGen = fopen(RANDOM_DEV, "rb");
	
	//fish the first digit out of the buffer.
	char digit = getc(randGen);


	// trapped until a valid digit appears.
	while(!isdigit(digit) && !isalpha(digit))
		digit = getc(randGen);

	//close open resources
	fclose(randGen);


	return digit;
}


/*

	Contructs a pseudo-random key of <siz> bytes.
	
*/
uint8_t *getRandomKey(int siz)
{
	uint8_t *key = (uint8_t *)malloc(siz);

	//parse and create.
	for (int i = 0; i < siz; i++)
	{
		*(key +i) = getRandomDigit();
	}

	printf("%s\n",key );
	return key;

	
}


/*
	Formats a string in order to preserve ONLY 0-9, A-Z , a-z.
	More "crypto.h"
*/
char *formatInput(char *inp, char *out)
{
	//assume dummies will handle the code.
	if(!inp)
		return NULL;

	//offset counters, usage of separate offset counter for in,out makes control much more precise..
	//[think about the issue where we receive <space> as inp[0]..] !

	size_t posIn = 0;
	size_t posOut = 0;
	
	//reserve some space.
	
	out = (char *)malloc(sizeof(char)*strlen(inp)+1);
	
	//parse the input.
	while(posIn < strlen(inp))
	{
		//if legit, copy respective digit or char on output, else just skip.
		if(isdigit(*(inp + posIn)) || isalpha(*(inp + posIn)))
		{	
			*(out + posOut) = *(inp + posIn);
			posOut++;
		}
		posIn++;

	}

	//manually terminate string.
	*(out + posOut) = '\0';

	return out;



}



/*
	Reads user's input (STDIN) dynamically with the use of realloc,malloc. 
	Parses the input character - wise and then decides whether reallocation of memory needs to be done [run - time].
	Args: < * inp> a pointer to where data retrieved from STDIN wil be stored.

*/

char *readInput()
{
	//allocating a fixed length buffer and a ctr to keep track of the string's size.
	unsigned int actualLength = 0;
	char *inp = (char *)malloc(sizeof(char )*BUF_SIZE);

	//the current character.
	char curr;
	
	//parse STDIN character-wise until newline is reached.
	while((curr = getchar()) != '\n')
	{
		//if limits are reached, reallocation must happen.
		if(actualLength >= BUF_SIZE)
			inp = realloc(inp, (actualLength + 1) * sizeof(char));

		//increment counter and store		
		inp[actualLength] = curr;
		actualLength++;
		

	}

	//not much for error handling but at least there's is one..
	if(!inp)
		return NULL;
	return inp;



}

