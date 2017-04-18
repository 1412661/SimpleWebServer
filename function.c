#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <regex.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "const.h"
#include "linklist.h"
#include "function.h"

/**
 * Alert and halt program for critical errors
 * @param format of output strings
 * @param list of variable for output
 */
//void error(const char *format, ...)
//{
    /**
    * Example use:
    * error("Could not open file", "test.txt", " for writing");
    * Output:
    * Critical error: Could not open file test.txt for writing
    */

//    va_list arg;

//    va_start(arg, format);
//    fprintf(stderr, "Critical error: ");
//    vfprintf(stderr, format, arg);
//    fprintf(stderr, "\n");
//    va_end(arg);

//    exit(EXIT_FAILURE);
//}

/**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding)
{
    /**
    * Example use:
    * char str1[] = "abc";
    * char* str2 = clone(str1, 3, 1);
    * str2[3] = '\0';
    * Sometime, str1 is in raw format without string termination character '\0'
    */
    return memcpy((char*)malloc(sizeof(char)*(size+padding)), buffer, size);
}

/**
 * Convert hex to dec by using strtol() without modified memory
 * @param Hex string
 * @param Pointer to the last character of the hex string
 * @return Dec value
 */
unsigned int hex(char* str, char* end)
{
    // Because strtol() will modify memory,
    // we have to use temporary memory space for input
    char* tmp = clone(str, end-str+1, 0);
    tmp[end-str] = '\0';

    unsigned int result = strtol(tmp, NULL, 16);

    free(tmp);

    return result;
}

/**
 * Check if string satisfy regular expression (POSIX extended standard)
 * @param Input string
 * @param Regular expression
 * @return 0 if matched, 1 if not
 */
int regexCheck(char* data, char* regexString)
{
    // Max matching group (only need two, one for full group, one for child group)
    int tmp = 2;

    struct List* list = parseByRegex(regexString, data, &tmp);
    if (list == NULL)
        return 0;

    freeList(&list);

    return 1;
}

/**
 * Parse string into several parts by regular expression (POSIX extended standard)
 * @param Regular expressing string
 * @param String need to be parsed
 * @param Maximum capture group. If null, REGEX_MAX_MATCH is used.
 *        Return actual capture groups when finish.
 * @return Linked list that store both full capture group and child capture group.
 */
struct List* parseByRegex(char* regexString, char* string, unsigned int* nCaptureGroup)
{
	// If nCaptureGroup != 0, max capture group (regexMaxMatch) is nCaptureGroup
	// otherwise use REGEX_MAX_MATCH
    unsigned int regexMaxMatch = REGEX_MAX_MATCH;
    if (nCaptureGroup != NULL)
        if (*nCaptureGroup != 0)
        {
            regexMaxMatch = *nCaptureGroup;
            *nCaptureGroup = 0;
        }

    regex_t regex;
    regmatch_t *pmatch = (regmatch_t*)malloc(sizeof(regmatch_t)*regexMaxMatch);

    //REG_EXTENDED: Use POSIX Extended Regular Expression
    if (regcomp(&regex, regexString, REG_EXTENDED))
        printf("parseByRegex(): Could not compile regex: %s\n", regexString);

    if (regexec(&regex, string, regexMaxMatch, pmatch, 0))
    {
        regfree(&regex);
        free(pmatch);
        return NULL;
    }

    regfree(&regex);

    struct List* r = NULL;
    int i = 0;
    for (i = 0; i < regexMaxMatch; i++)
    {
        if (pmatch[i].rm_so < 0)
            break;

        int len = pmatch[i].rm_eo - pmatch[i].rm_so;			// Get length of matching group
        char* str = clone(string + pmatch[i].rm_so, len, 1);	// Clone matching group into new address space
        str[len] = '\0';

        struct List* tmp = newList(str);						// Insert matching group to linked list
        insertList(&r, tmp);

        if (nCaptureGroup != NULL)								// nCaptureGroup is used to count captured matching groups
            (*nCaptureGroup)++;
    }

    free(pmatch);

    return r;
}



char* getRequestFile(char* msg)
{
	char* file;

	if (strstr(msg, "GET / HTTP") != NULL)
	{
		file = (char*)malloc(BUFFSIZE_VAR);
        strcpy(file, INDEX_FILE);
        return file;
	}

	char* first = strstr(msg, "/") + 1;
    char* last = strstr(first, " ") - 1;

    file = clone(first, last-first+1, 1);
    file[last-first+1] = '\0';

	return file;
}



int countChildProcess(int parentPid)
{
	char command[BUFFSIZE_VAR] = "";
	char countc[BUFFSIZE_VAR];

    sprintf(command, "ps --ppid %d --no-headers | grep -v defunct | wc -l", parentPid);

	FILE* f = popen(command, "r");
    fgets(countc, sizeof(countc), f);

    return strtol(countc, NULL, 10) - 1;
}
