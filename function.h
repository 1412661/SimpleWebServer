#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <stdarg.h>
#include "const.h"

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
 * Count child processes of a parent process
 * @param Parent process ID
 * @return Number of child process (exclude sh)
 */
int countChildProcess(int parentPid);

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

char* readFile(char* file);
char* extractRequest(char* mesg);

int ishex(int x);
int decode(char *s, char *dec);

//char* char_replace(char search, char replace, char* subject)
//char *str_replace(char *search , char *replace , char *subject);

#endif // _FUNCTION_H_
