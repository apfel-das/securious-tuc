#define _GNU_SOURCE

#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef LOFFILE
#define LOGFILE "file_logging.log"
#endif


void log_actions(char *logPath, int uid, const char *fPath, struct tm *timeInfo, int open, int denied, int exists,unsigned char *hash);
struct tm *get_time();

int fileExists(const char *fName);

char * getFilePath(const char *fName);









/*
	A tricky implementation of fopen, made to log actions.

*/

FILE *fopen(const char *path, const char *mode) 
{

	
	FILE *original_fopen_ret;
	FILE *(*original_fopen)(const char*, const char*);

	/* call the original fopen function */
	original_fopen = dlsym(RTLD_NEXT, "fopen");
	original_fopen_ret = (*original_fopen)(path, mode);

	/*
		Gather up useful insights about the action
	
	*/

	//checking whether fopen() denied or not.
	int denied = (!original_fopen_ret) ? 1 : 0;

	//getting the abs. file path, or NULL.
	char *fPath = (fileExists(path)) ? (getFilePath(path)) : NULL;

	
	
	
	//attempt to log all stats concerning the call.
	log_actions(LOGFILE,getuid(),fPath,get_time(), 0, denied, fileExists(path), NULL);
	

	//free resources.
	free(fPath);


	/*
	
		Do what the actual function would do.

	*/


	//return the original so the open proc, actually happens.

	return original_fopen_ret;
}






/*
	A tricky implementation of fwrite, made to log actions.

*/


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
{

	size_t original_fwrite_ret;
	size_t (*original_fwrite)(const void*, size_t, size_t, FILE*);

	/* call the original fwrite function */
	original_fwrite = dlsym(RTLD_NEXT, "fwrite");
	original_fwrite_ret = (*original_fwrite)(ptr, size, nmemb, stream);


	/* add your code here */
	/* ... */
	/* ... */
	/* ... */
	/* ... */

	//return the original so the write proc, actually happens.

	return original_fwrite_ret;
}



/*
		Logs user's actions by writing on file specified by <char *logPath>


*/

void log_actions(char *logPath, int uid, const char *fPath, struct tm *timeInfo, int open, int denied, int exists,unsigned char *hash)
{

	//printing on debug.
	
	fprintf(stderr, "%s\t%-50s\t%s\t%s\t%s\n", "UID", "File Path", "Year","Denied","File Exists");
	fprintf(stderr, "%-4d\t%-50s\t%-4d\t%-4d\t%4d\n", uid, fPath, timeInfo->tm_year, denied,exists);



}


/*

	Returns system time when called.

	Note:

		 - When in Ubuntu tm_year + 1900 will produce the proper year.
		 - Function handles that field change as well.

*/

struct tm *get_time()
{

	struct tm *retVal;
	time_t rawtime;

	//get time, adjust on local settings.
	time(&rawtime);
	retVal = localtime(&rawtime);

	//fix calibration issue by adding 1900 to the result.
	retVal->tm_year += 1900;


	return retVal;
}





/*
	
	Returns the full file path, showed by <int fp>.
	Also handles files that cannot open as file_descriptor cause user has insufficinet priviledges.

	Args:
			- The file pointer specifying the file.
	Returns:

			- The file path as a char array/string or NULL if failure.
	Note:

			- Upon failure caller must hadle the NULL pointer returned.
			- This will not work on Windows or MacOS [if MacOS is your target handle with fcntl()]
			- This is similar to readlink -e, yet realpath is a NULL TERMINATING instead of afforementioned realpath.
			- This implementation solves the issue of what to do if user has no priviledge to open file, since no file descriptor is used here.

	


*/

char *getFilePath(const char *fName)
{
	char *path = realpath(fName, NULL);
	
	//Return either absolute path or NULL.
	if(!path)
		return NULL;
	return path;
}


/*
	
	Returns 1 if file exists, 0 if not.
	Args:
			- The filePath as <char *>.
	Notes:
			- Do not use <access()> for checking existance, several issues concerning safety might occur.

	
*/

int fileExists(const char *fName)
{
	struct stat b;
	return (stat(fName, &b) == 0);
}

