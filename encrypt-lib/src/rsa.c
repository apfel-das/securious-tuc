#include "rsa.h"
#include "utils.h"


/*
 * Sieve of Eratosthenes Algorithm
 * https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 *
 * arg0: A limit
 * arg1: The size of the generated primes list. Empty argument used as ret val
 *
 * ret:  The prime numbers that are less or equal to the limit
   Note: This has an aprox. time complexity of O(n*nlog(logn)) which is SHIT. I WILL SOMETIME EDIT IT.
   Note2: A more comprehensible impmlementation would be the use of math.h, but that isn't of my personal taste.
 */
size_t *sieve_of_eratosthenes(int limit, int *primes_sz)
{
	size_t *primes;
	size_t numPool[limit  + 1];
	int primeCount = 0;

	//initialize the prime pool.
	for (int i = 0; i < limit; i++)
	{
		//values below 2, will be assigned with 0, others start from 1.
		(i < 2) ? (numPool[i] = 0) : (numPool[i] = 1);
		
	}

	//count increments, mark multiples as processed and non - fit.
	for(int i = 2; i*i <= limit; i++)
	{
		//exclude multiples of i.
		if(numPool[i] == 1)
		{
			for(int j = i*i; j <= limit; j += i)
				numPool[j] = 0;
		}
	}
	

	//define the length.
	for(int i = 2; i <= limit; i++)
	{
		//if not 0 in pool, we have ourselves a prime gentlemen, lets count him.
			if(numPool[i])
			{
				primeCount++;
			}
	}

	//alloc. some space.
	primes = (size_t *)malloc(sizeof(size_t)*primeCount);
	int pc = 0;

	//copy primes into return location.

	for(int i = 2; i <= limit; i++)
	{
		//if not 0 in pool, we have ourselves a prime gentlemen, lets count him.
			if(numPool[i])
			{
				*(primes + pc) = i;
				pc++;
			}
	}


	//inform about size.
	*primes_sz = primeCount;

	//return array of primes.
	return primes;
}





/*
 * Greatest Common Denominator
 *
 * arg0: first number
 * arg1: second number
 *
 * ret: the GCD
 */
int gcd(int a, int b)
{

	int common;

	while(a != 0)
	{
		common = a;
		a = b % a;
		b = common;

	}

	return common;
}


/*
 * Chooses 'e' where 
 *     1 < e < fi(n) AND gcd(e, fi(n)) == 1
 *
 * arg0: fi(n)
 *
 * ret: 'e'
 */
size_t choose_e(size_t fi_n, size_t *primePool, int poolSize)
{
	size_t curr = 0;


	for(int i = 0; i < poolSize; i++)
	{
		//get current prime from pool, return the first tha satisfies selection criteria.
		curr = primePool[i];
		if((curr > 3) && (curr < fi_n) && (gcd(curr,fi_n) == 1))
			return curr;
	}

	
	//shouldn't reach here..
	return -1;
}


/*
 * Calculates the modular inverse
 *
 * arg0: first number
 * arg1: second number
 *
 * ret: modular inverse
 */
size_t mod_inverse(size_t a, size_t b)
{

	//fast and memory efficient way..
	a = a%b;

	for(size_t i = 1; i < b; i++)
	{
		if( (a*i) % b == 1)
		{
			return i;
		}

	}


	//shouldn't reach here..
	return -1;
}

/*
	Modular exponentiatiation between base,power,reduction factor.
	Calculates b^exp mod m.
	Returns: Calculated exponentiation.
	Note: That's the quick, yet bottlenecked way of doing it, but it will work for BIG exponents.
	Hint: If wishing to stress execute mod_expo(3,1024,7) should be 4.
*/

size_t mod_expo(size_t b, size_t exp, size_t m)
{
	//failure sign.
	if(b < 0 || exp < 0 || m <= 0)
		exit(-1);
	
	b = b % m;
	
	//1 mod anything -> anything
	if(exp == 0) 
		return 1;

	//pure modulo division
	if (exp == 1) 
		return b;
	
	//powers in play, C's power is recursion, lets do that!
	if(exp % 2 == 0)
		return (mod_expo(b * (b%m), exp/2, m) % m);
	else
		return (b*mod_expo(b , exp-1, m) % m);


}


/*
 * Generates an RSA key pair and saves
 * each key in a different file
 */
void rsa_keygen(void)
{
	size_t p;
	size_t q;
	size_t n;
	size_t fi_n;
	size_t e;
	size_t d;

	/* TODO */

}


/*
 * Encrypts an input file and dumps the ciphertext into an output file
 *
 * arg0: path to input file
 * arg1: path to output file
 * arg2: path to key file
 */
void rsa_encrypt(char *input_file, char *output_file, char *key_file)
{

	/* TODO */

}


/*
 * Decrypts an input file and dumps the plaintext into an output file
 *
 * arg0: path to input file
 * arg1: path to output file
 * arg2: path to key file
 */
void rsa_decrypt(char *input_file, char *output_file, char *key_file)
{

	/* TODO */

}
