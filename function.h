#ifndef _FUNCTION_H_
#define _FUNCTION_H_

/**
 * @file function.h
 * @brief Provide function prototype
 */

/**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding);

/**
 * Get request file in HTTP request
 * @param HTTP request message
 * @return Requested file
 */
char* getRequestFile(char* msg);

/**
 * Search captital by country
 * @param country
 * @return capital of the country, NULL if couldn't find
 */
char* searchCap(char* country);


/**
 * Get country name in HTTP request
 * @param HTTP request message
 * @return Requested country, NULL if couldn't find
 */
 char* getRequestCountry(char* msg);

 /**
 * Read file into memory
 * @param Path to file
 * @return File content (terminated by NULL), NULL if file is not exist
 */
char* readFile(char* file);


/**
 * Get the first line of a HTTP request
 * @param HTTP request
 * @return First line of the HTTP request
 */
char* extractRequest(char* mesg);


// URL decoder
/**
 * Test if a chracter is in hex format: 0123456789ABCDEF
 * @param A character that need to be test
 * @return 1 for true, 0 for false
 */
int ishex(int x);

/**
 * Decode a query string in HTTP/GET request
 * Example: Vi%26%237879%3Bt+Nam will become Việt Nam
 * @param Input query string
 * @param Output query string (need to be allocated first) or NULL
 * @return A decoded query string. If the second param is NULL,
 *		   length of the decoded query string will be returned
 */
int decode(char *s, char *dec);

/**
 * Find a free thread handler in marking array
 * @param Marking array
 * @return index of the free thread, -1 if all handler is busy.
 */
int findEmptyThread(int *thread);


//char* char_replace(char search, char replace, char* subject)
//char *str_replace(char *search , char *replace , char *subject);

#endif // _FUNCTION_H_
