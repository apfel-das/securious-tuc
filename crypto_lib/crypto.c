#include "crypto.h"
#include <fcntl.h> 
#include <unistd.h>
//function declarations.


#define printable 32  // an offset to reach in the printable ASCII area [32 - 197 in decimal]

int main()
{

	char *cipher = NULL;
	char *key = NULL;
	char *out = NULL;
	char *orig = NULL;

	printf("Hello crypto-curious..\n");
	

	printf("plaintext: ");
	cipher = readInput();
	cipher = formatInput(cipher, out);
	printf("Input check gave:%s\n",cipher);
	


	return 0;
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



// Todo: Fix issues!! Make it more sophisticated..

void encrypt(char *inp, char *out, char *key)
{

	for (int i = 0; i < strlen(inp); ++i)
	{
		// only alphanumeric characters get to be converted.
		if(isalpha(*(inp+i)) || isdigit(*(inp + i)))
		{
			
				int f = (*(inp + i))^(*(key + i));
				printf("Original: %s Char: %c %d  Char_k : %c %d XOR: %d\n",inp, *(inp+i), *(inp+i), *(key+i), *(key+i), f);

				*(out + i) = (char)f;
			
			
		}
		else
		{
			//non convertable character, just copy.
			*(out + i) = *(inp +i);
		}

	}
	


	//manually append string termination character at end.
	*(out + strlen(inp)) = '\0';

}

void decrypt(char *inp, char *out, char *key)
{

	//parse the cipher 
	for (int i = 0; i < strlen(inp); ++i)
	{
		
	
		// XOR key and input in respective offset.
		int f = (*(key + i))^(*(inp + i));
		
		// let the user know some.
		printf("Original: %s Char: %c %d  Char_k : %c %d XOR: %d\n",inp, *(inp+i), *(inp+i), *(key+i), *(key+i), f);

		*(out + i) = (char)f; 
	
			
		

	}
	


	//manually append string termination character at end.
	*(out + strlen(inp)) = '\0';

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
		exit(-1);
	printf("We've read %s\n",inp );

	return inp;



}

