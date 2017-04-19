#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <stdarg.h>
#include "const.h"

/**
 * @file function.h
 * @brief Provide function prototype
 */

/**
 * Alert and halt program for critical errors
 * @param format of output strings
 * @param list of variable for output
 */
void error(const char *format, ...);

/**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding);

/**
 * Parse string into several parts by regular expression (POSIX extended standard)
 * @param Regular expressing string
 * @param String need to be parsed
 * @param Maximum capture group. If null, REGEX_MAX_MATCH is used.
 *        Return actual capture groups when finish.
 * @return Linked list that store both full capture group and child capture group.
 */
struct List* parseByRegex(char* regexString, char* string, unsigned int* nCaptureGroup);

/**
 * Convert hex to dec by using strtol() without modified memory
 * @param Hex string
 * @param Pointer to the last character of the hex string
 * @return Dec value
 */
unsigned int hex(char* str, char* end);

/**
 * Check if string satisfy regular expression (POSIX extended standard)
 * @param Input string
 * @param Regular expression
 * @return 0 if matched, 1 if not
 */
int regexCheck(char* data, char* regexString);

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

#endif // _FUNCTION_H_
