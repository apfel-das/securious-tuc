#include "utils.h"
#ifndef BUF_SIZ
#define BUF_SIZ 512
#endif

/*
 * Prints the hex value of the input
 *
 * arg0: data
 * arg1: data len
 */
void print_hex(unsigned char *data, size_t len)
{
	size_t i;

	if (!data)
		printf("NULL data\n");
	else {
		for (i = 0; i < len; i++) {
			if (!(i % 16) && (i != 0))
				printf("%02X ", data[i]);
			printf("%02X ", data[i]);
		}
		printf("\n");
	}
}


/*
 * Prints the input as string
 *
 * arg0: data
 * arg1: data len
 */
void print_string(unsigned char *data, size_t len)
{
	size_t i;

	if (!data)
		printf("NULL data\n");
	else {
		for (i = 0; i < len; i++)
			printf("%c", data[i]);
		printf("\n");
	}
}


/*
 * Prints the usage message in a kinda nice manner.
 */
void usage(void)
{
	printf(
	    "\n"
	    "Usage:\n"
	    "    assign_3 -g \n" 
	    "    assign_3 -i in_file -o out_file -k key_file [-d | -e]\n" 
	    "    assign_3 -h\n"
	);
	printf(
	    "\n"
	    "Options:\n"
	    " -i    path    Path to input file\n"
	    " -o    path    Path to output file\n"
	    " -k    path    Path to key file\n"
	    " -d            Decrypt input and store results to output\n"
	    " -e            Encrypt input and store results to output\n"
	    " -g            Generates a keypair and saves to 2 files\n"
	    " -h            This help message\n"
	);
	exit(EXIT_FAILURE);
}


/*

 * Checks the validity of the arguments given
 *
 * arg0: path to input file
 * arg1: path to output file
 * arg2: path to key file
 * arg3: operation mode


 */
void check_args(char *input_file, char *output_file, char *key_file, int op_mode)
{
	if ((!input_file) && (op_mode != 2)) {
		printf("Error: No input file!\n");
		usage();
	}

	if ((!output_file) && (op_mode != 2)) {
		printf("Error: No output file!\n");
		usage();
	}

	if ((!key_file) && (op_mode != 2)) {
		printf("Error: No user key!\n");
		usage();
	}

	if (op_mode == -1) {
		printf("Error: No mode\n");
		usage();
	}
}

/*
	Parses and reads the content of a file, specified by <char *fPath>.
	Arguments:
				<char *fPath>:			full path of the file to be read.
				<unsigned long *len>: 	pointer to a location, whre the size of data in bytes will be stored.
	Returns:
				Data read as an <unsigned char *> [a form of string in C].
*/

unsigned char *readFile(char *fPath, unsigned long *len)
{
	FILE *fp;
	long fileLen = 0;
	

	//allocate some space on a buffer.
	unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char )*BUF_SIZ + 1);


	//open the file, via the file pointer.

	fp  = fopen(fPath, "rb");
	
	//error handling on dummy file location.
	if(fp == NULL)
	{
		fprintf(stderr, "Error on opening file, tool will terminate now..\n" );
		exit(EXIT_FAILURE);
	}

	//estimate the length.
	fseek(fp, 0, SEEK_END);

	//unsafe yet useful for our cause.
	fileLen = ftell(fp);

	//set on beginning of file.
	rewind(fp);

	//read the first character.
	char ch = getc(fp);
	int charCount = 0;


	/*

		Checking for EOF [ch != EOF] will work on normal ASCII files.
		For reading the encrypted this won't work cause encrypted chars might contain EOF [0xffff] under some OS's.
		So it is prefered to check for file length via the estimation of ftell() even if ftell() is partially unsafe [especially under Windows].

	*/
	while(charCount != fileLen)
	{
		
		//re-allocate if needed.
		if(charCount >= BUF_SIZ)
			data = realloc(data, (charCount + 1)*sizeof(unsigned char));

		//counter incrementation and storage.
		data[charCount] = ch;
		charCount++;

		//read next char.
		ch = getc(fp);



	}
	
	//some error handling and then read.
	if(!data)
	{
		fprintf(stderr, "Error on initializing memory, tool will terminate now..\n" );
		exit(EXIT_FAILURE);
	}



	//append termination
	*(data + charCount) = '\0';

	//inform about the size
	*len = charCount;

	//avoid leaks.
	fclose(fp);


	return data;



}





/*
	
	Writes data in an output file.

	Arguments:
				<char *fPath>:			full path of the file to be filled.
				<unsigned long *len>: 	pointer to a location, where the size of data in bytes will be retrieved.
	Returns:
				None.
	Note:
				<unsigned long len>: should not be NULL, should have a valid length.

*/
void writeFile(char *fPath, unsigned char *data, unsigned long len)
{

	//initialise a file pointer;
	FILE *fp;
	

	// opt will determine whether bytes or ASCII will be written.

	fp = fopen(fPath, "wb");

	if(!fp)
	{
		fprintf(stderr, "File not found, tool will terminate now..\n" );
		exit(-1);
	}

	///write as a stream and not character wise.
	fwrite(data, sizeof(unsigned char) ,(size_t)len, fp);

	fclose(fp);
}




