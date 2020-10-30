#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/cmac.h>

#define BLOCK_SIZE 16


/* Utility function prototypes */


void print_hex(unsigned char *, size_t);
void print_string(unsigned char *, size_t); 
void usage(void);
void check_args(char *, char *, unsigned char *, int, int);
unsigned char *readFile(char *fPath, unsigned long *len);
void writeFile(char *fPath, unsigned char *data, unsigned long *len);

/* Actual encryption functions */

void keygen(unsigned char *, unsigned char *, unsigned char *, int);
void encrypt(unsigned char *, int, unsigned char *, unsigned char *, unsigned char *, int );
int decrypt(unsigned char *, int, unsigned char *, unsigned char *, unsigned char *, int);
void gen_cmac(unsigned char *, size_t, unsigned char *, unsigned char *, int);
int verify_cmac(unsigned char *, unsigned char *);





/* TODO Declare your function prototypes here... */



/* Variables */

unsigned char globalKey[256];
unsigned char iv[256];

/*
 * Prints the hex value of the input
 * 16 values per line
 */
void print_hex(unsigned char *data, size_t len)
{
	size_t i;

	if (!data)
		printf("NULL data\n");
	else {
		for (i = 0; i < len; i++) {
			if (!(i % 16) && (i != 0))
				printf("\n");
			printf("%02X ", data[i]);
		}
		printf("\n");
	}
}


/*
 * Prints the input as string
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
 * Prints the usage message
 * Describe the usage of the new arguments you introduce
 */
void usage(void)
{
	printf(
	    "\n"
	    "Usage:\n"
	    "    assign_1 -i in_file -o out_file -p passwd -b bits" 
	        " [-d | -e | -s | -v]\n"
	    "    assign_1 -h\n"
	);
	printf(
	    "\n"
	    "Options:\n"
	    " -i    path    Path to input file\n"
	    " -o    path    Path to output file\n"
	    " -p    psswd   Password for key generation\n"
	    " -b    bits    Bit mode (128 or 256 only)\n"
	    " -d            Decrypt input and store results to output\n"
	    " -e            Encrypt input and store results to output\n"
	    " -s            Encrypt+sign input and store results to output\n"
	    " -v            Decrypt+verify input and store results to output\n"
	    " -h            This help message\n"
	);
	exit(EXIT_FAILURE);
}


/*
 * Checks the validity of the arguments
 * Check the new arguments you introduce
 */
void check_args(char *input_file, char *output_file, unsigned char *password,  int bit_mode, int op_mode)
{
	if (!input_file) {
		printf("Error: No input file!\n");
		usage();
	}

	if (!output_file) {
		printf("Error: No output file!\n");
		usage();
	}

	if (!password) {
		printf("Error: No user key!\n");
		usage();
	}

	if ((bit_mode != 128) && (bit_mode != 256)) {
		printf("Error: Bit Mode <%d> is invalid!\n", bit_mode);
		usage();
	}

	if (op_mode == -1) {
		printf("Error: No mode\n");
		usage();
	}
}


/*
 * Generates a key using a given password.
   Arguments:	
   					<char *password>: A password under which the key gets generated.
   					<char *key>: 	  Points the location in which the generated key gets to be stored.
   					<char *iv>:
   					<char *bit_mode>: The actual bits of the AES-ECB to be produced.
   	Notes: 
   					- <char *bit_mode> is guaranteed to be 128 or 256.
					- sha1 hashing is applied.
					- no "salt" is used.
 */
void keygen(unsigned char *password, unsigned char *key, unsigned char *iv, int bit_mode)
{



	//defining parameters.
	const unsigned char *salt = NULL;
	const EVP_CIPHER *cipher;
	const EVP_MD *hash = EVP_get_digestbyname("sha1");


	//getting the cipher in respect to the bit mode.
	(bit_mode == 128) ? (cipher = EVP_get_cipherbyname("aes-128-ecb")): (cipher = EVP_get_cipherbyname("aes-256-ecb"));

	//generating key and error handling.

	if (EVP_BytesToKey(cipher, hash, salt, (unsigned char *)password, strlen((char *)password), 1, globalKey, iv) == 0)
	{
		fprintf(stderr, "Error in key generation..\n");
		exit(-1);
	}
	

}


/*
 * Encrypts the data
 */
void encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int bit_mode)
{

	printf("%s\n", plaintext);
	//parameters
	EVP_CIPHER_CTX *context; 
	const EVP_CIPHER *cipher;
	int encryptionLength;

	//init. the context.
	context =  EVP_CIPHER_CTX_new();

	//getting the cipher with respect on the bit mode.
	(bit_mode == 128) ? (cipher = EVP_get_cipherbyname("aes-128-ecb")): (cipher = EVP_get_cipherbyname("aes-256-ecb"));

	//be brave, apply assertion. [remove if feeling sure about the results]
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(context) == (bit_mode % 8));


	//init. encryption process and error handling.
	if(EVP_EncryptInit_ex(context, cipher, NULL, key, iv) == 0)
	{
		fprintf(stderr, "Error upon context initialization [Encryption]\n");
		exit(-1);
	}


	//updating the encryption, error handling as well.

	if(!EVP_EncryptUpdate(context, ciphertext, &encryptionLength, plaintext, plaintext_len))
	{
		EVP_CIPHER_CTX_free(context);
		fprintf(stderr, "Error upon updating context [Encryption]\n");
		exit(-1);
	}


	//finalising encryption process with error handling as always.
	if(!EVP_EncryptFinal_ex(context, ciphertext + encryptionLength, &encryptionLength))
	{
		EVP_CIPHER_CTX_free(context);
		fprintf(stderr, "Error upon finalising the encryption process [Encryption]\n");
		exit(-1);


	}

	//releasing sensitive info from memory.
	EVP_CIPHER_CTX_free(context);

	printf("Done Encrypting..\n");
	print_hex(ciphertext, (size_t)encryptionLength);
	print_string(ciphertext, (size_t)encryptionLength);

}


/*
 * Decrypts the data and returns the plaintext size
 */
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int bit_mode)
{
	int plaintext_len = 0;
	
	//parameters
	EVP_CIPHER_CTX *context; 
	const EVP_CIPHER *cipher;
	int encryptionLength;

	//init. the context.
	context =  EVP_CIPHER_CTX_new();

	//getting the cipher with respect on the bit mode.
	(bit_mode == 128) ? (cipher = EVP_get_cipherbyname("aes-128-ecb")): (cipher = EVP_get_cipherbyname("aes-256-ecb"));

	//be brave, apply assertion. [remove if feeling sure about the results]
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(context) == (bit_mode % 8));


	//init. encryption process and error handling.
	if(EVP_DecryptInit_ex(context, cipher, NULL, key, iv) == 0)
	{
		fprintf(stderr, "Error upon context initialization [Decryption]\n");
		exit(-1);
	}


	//updating the encryption, error handling as well.

	if(!EVP_DecryptUpdate(context, plaintext, &encryptionLength, ciphertext, ciphertext_len))
	{
		EVP_CIPHER_CTX_free(context);
		fprintf(stderr, "Error upon updating context [Decryption]\n");
		exit(-1);
	}

	//update count.
	plaintext_len = encryptionLength;

	//finalising encryption process with error handling as always.
	if(!EVP_DecryptFinal_ex(context, plaintext + encryptionLength, &encryptionLength))
	{
		EVP_CIPHER_CTX_free(context);
		fprintf(stderr, "Error upon finalising the encryption process [Decryption]\n");
		exit(-1);


	}
	//finalize length.
	plaintext_len += encryptionLength;

	//releasing sensitive info from memory.
	EVP_CIPHER_CTX_free(context);

	printf("Done Decrypting..\n");
	print_hex(plaintext, (size_t)encryptionLength);
	print_string(plaintext, (size_t)plaintext_len);
	printf("%s\n", plaintext);
	



	return plaintext_len;
}


/*
 * Generates a CMAC
 */
void
gen_cmac(unsigned char *data, size_t data_len, unsigned char *key, 
    unsigned char *cmac, int bit_mode)
{

	/* TODO Task D */

}


/*
 * Verifies a CMAC
 */
int
verify_cmac(unsigned char *cmac1, unsigned char *cmac2)
{
	int verify;

	verify = 0;

	/* TODO Task E */

	return verify;
}



/* Utility functions */



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
	unsigned char *data;
	size_t retrieved = 0;

	//open the file, via the file pointer.

	fp  = fopen(fPath, "rb");
	
	//error handling on dummy file location.
	if(!fp)
	{
		fprintf(stderr, "File not found, tool will terminate now..\n" );
		exit(-1);
	}
	
	//get the length of the file.
	fseek(fp, 0, SEEK_END);
	*len = ftell(fp);

	//re-calibrate, goto start.
	fseek(fp, 0, SEEK_SET);

	//allocate some space.
	data  = (unsigned char *)malloc(*len);

	//some error handling and then read.
	if(data)
		retrieved = fread(data, 1, *len, fp);

	if(!retrieved)
		return NULL;
	return data;


}


/*
	
	Writes data in an output file.

*/
void writeFile(char *fPath, unsigned char *data, unsigned long *len)
{

	//initialise a file pointer;
	FILE *fp;

	fp = fopen(fPath, "wb");

	if(!fp)
	{
		fprintf(stderr, "File not found, tool will terminate now..\n" );
		exit(-1);
	}

	fwrite(data, 1 ,(size_t)len, fp);

}



/* TODO Develop your functions here... */



/*
 * Encrypts the input file and stores the ciphertext to the output file
 *
 * Decrypts the input file and stores the plaintext to the output file
 *
 * Encrypts and signs the input file and stores the ciphertext concatenated with 
 * the CMAC to the output file
 *
 * Decrypts and verifies the input file and stores the plaintext to the output
 * file
 */
int main(int argc, char **argv)
{
	int opt;			/* used for command line arguments */
	int bit_mode;			/* defines the key-size 128 or 256 */
	int op_mode;			/* operation mode */
	char *input_file;		/* path to the input file */
	char *output_file;		/* path to the output file */
	unsigned char *password;	/* the user defined password */

	//usefull when holding input, output.

	unsigned char *input = NULL;
	unsigned char *output = NULL;
	unsigned long inpLen = 0;
	unsigned long outpLen = 0;







	/* Init arguments */
	input_file = NULL;
	output_file = NULL;
	password = NULL;
	bit_mode = -1;
	op_mode = -1;


	/*
	 * Get arguments
	 */
	while ((opt = getopt(argc, argv, "b:i:m:o:p:desvh:")) != -1) {
		switch (opt) {
		case 'b':
			bit_mode = atoi(optarg);
			break;
		case 'i':
			input_file = strdup(optarg);
			break;
		case 'o':
			output_file = strdup(optarg);
			break;
		case 'p':
			password = (unsigned char *)strdup(optarg);
			break;
		case 'd':
			/* if op_mode == 1 the tool decrypts */
			op_mode = 1;
			break;
		case 'e':
			/* if op_mode == 1 the tool encrypts */
			op_mode = 0;
			break;
		case 's':
			/* if op_mode == 1 the tool signs */
			op_mode = 2;
			break;
		case 'v':
			/* if op_mode == 1 the tool verifies */
			op_mode = 3;
			break;
		case 'h':
		default:
			usage();
		}
	}


	/* check arguments */
	check_args(input_file, output_file, password, bit_mode, op_mode);

	


	/// Free the input buffer, then do stuff.

	if(input_file != NULL)
		input= readFile(input_file, &inpLen);
	if(!input)
	{
		fprintf(stderr, "Empty data set given, exiting now..\n");
		exit(-1);
	}
	


	/*

	Initialize the library, no need to be done in version above OpenSSL 1.1.0
	Uncomment if running versions below 1.1.0
	
	*/
	//OpenSSL_add_all_algorithms(); 


	/* Keygen from password */

	printf("Input:\t%s\n", input);

	printf("Password:%s\n", password);
	keygen(password, globalKey, iv, bit_mode);

	printf("Key:%s Length[bits]: %ld \n", globalKey, strlen((const char *)globalKey)*8);
	/* Operate on the data according to the mode */

	switch(op_mode)
	{
		case 0:

				printf("About to encrypt..[Debug]\n");

				//strlen(output) = strlen(input) + strlen(padding) + strlen(CMACM)
				//allocate appropriate space.

				outpLen = inpLen - (inpLen % BLOCK_SIZE) + BLOCK_SIZE;
				output = (unsigned char *)malloc(outpLen);

				//encrypt..
				encrypt(input, inpLen, globalKey, iv, output, bit_mode);

				printf("Decrypting instantly..[Debug]\n");
				outpLen = decrypt(output, outpLen, globalKey, NULL, input, bit_mode);

				break;
		default:
				fprintf(stderr, "Invalid opt_mode..\n");	
				break;
	}








	/* encrypt */

	/* decrypt */

	/* sign */

	/* verify */
		

	/* Clean up */
	free(input_file);
	free(output_file);
	free(password);

	free(input);
	free(output);


	/* END */
	return 0;
}
