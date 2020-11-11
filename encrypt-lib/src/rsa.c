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
	printf("Eratosthenes is fine..\n");
	//return array of primes.
	return primes;
}





/*
 * Greatest Common Denominator
 *
 * arg0: first number
 * arg1: second number
 *
 * ret: the GCD, might be zero!!
 * note: I might change that later with Stein's Algo, seems kinda more efficient..
   Change: Recursive algo is a bit quicker I tend to believe..!
 */
int gcd(int a, int b)
{

	//recursive way.

	if(b == 0)
		return a;
	return gcd(b, a%b);

	
	

	
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

	for(int i = 1; i < b; i++)
	{
		if( ((a*i)%b) == 1)
		{
			return (size_t)i;
		}

	}


	//shouldn't reach here..
	return -1;
}

/*
	Modular exponentiatiation between base,power,reduction factor.
	Calculates b^exp mod m., where m is the reduction factor.
	

	Returns:	Calculated exponentiation, or exits upon parameter failure.
	Note: 		That's the quick, yet bottlenecked (due recursion) way of doing it, but it will work for BIG exponents.
	Hint: 		If wishing to stress test execute mod_expo(3,1024,7)/mod_expo(3,2048, 7) should be 4/2.
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

	Selects two random primes(p,q) from a pool given [therefore "fishing"].
	arg1: a pool to fish from.
	arg2: the size of fishing pool.
	arg3: pointer to p [can be NULL on call].
	arg4: pointer to q [can be NULL on call].
	Notes:

		- <size_t *primePool>/ <arg1>:	should be an initialized pointer to a structure of prime numbers.
		- <int pool_siz>/<arg2>:		should be also equal to the size of the pool [aka count of elements].
		- <size_t *p>, <size_t *q>/:	will be showing to random prime numbers after calling this function.
	
	Important:	Possible leak if <arg3>, <arg4> don't get freed by caller.
				Caller should ensure primePool is NOT NULL.



*/

void prime_fishing(size_t *primePool, int pool_siz, size_t *p, size_t *q)
{

	//random indexes on pool.
	int p_index, q_index;

	//assume dummies
	if(!primePool || !pool_siz)
	{
		fprintf(stderr, "Empty pool given..[key_gen]\n");
		exit(-1);
	}

	//seed random generator with current time, "/dev/urand" could also fit..
	//under some compilers (unsigned)time(..) might be a best fit..
	srand(time(NULL));

	//aquire indexes, set their limits in range.
	p_index = (rand() % (int)pool_siz);
	q_index =  (rand() % (int)pool_siz);

	//lock on equality.
	while(p_index == q_index)
	{
		//re-aquire.
		q_index =  (rand() % (int)pool_siz);
	}

	//altert the actual references..
	*p = p_index;
	*q = q_index;

	printf("Phising goes great..\n");



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
	size_t phi_n;
	size_t e;
	size_t d;

	//allocating space for 2 size_t quantities, represented as ASCII.
	size_t *key_buf = (size_t*)malloc(BASE_SIZ*2);


	
	/*

		Under the wrong pool or random number, non-suitable e's might result.
		Threfore we are trapped until validity comes out.
		Note:
			This might be a pain in the ass but it resolves a great deal of issues.
			Mostly <Floating point exception>

	*/
	do
	{
		//initialize a pool of RSA_SIEVE_LIMIT primes.
		int pool_siz;
		size_t *primePool = sieve_of_eratosthenes(RSA_SIEVE_LIMIT,&pool_siz);
		
		//initialize (p,q), random prime set.
		prime_fishing(primePool, pool_siz, &p, &q);

		//maths...
		n = p*q;
		phi_n = (p-1)*(q-1);
		e = choose_e(phi_n, primePool, pool_siz);

		
			
		}while(e == -1 || !(p && q) || (p == q));

		/*
			If -1 goes here as e, we get <floating point exception>.
			Therefore we trapped the previous chapter..
			
		*/
		d = mod_inverse(e, phi_n);

		//d should definetly be positive, in the unlike event it isn't we should adjust.
		while(d < 0)
			d = d + phi_n;


		/*
		
			Public vs Private key:
	
			n+d ->private  OR n+e->private
			n+e ->public      n+d->public


		*/

		//debug print, write on file..

		printf("public: %ld %ld\n", n,d);
		printf("private: %ld %ld\n", n,e);

		writeKey(n,d,"public.txt");
		writeKey(n,e, "private.txt");


		

	
	
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

	unsigned char *plaintext;
	unsigned char *key;

	size_t n,d;
	size_t *cipher;
	unsigned long plain_len, key_len;

	//read plaintext from file..
	plaintext = readFile(input_file, &plain_len);

	printf("Plain: %s\nL: %ld\n",plaintext, plain_len);

	//read the key as string..
	key = readFile(key_file, &key_len);

	//split part of key..
	memcpy(&n, key, sizeof(size_t));
	printf("N: %ld\n", n);






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
