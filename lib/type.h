#ifndef _TYPE_H_
#define _TYPE_H_

#include "const.h"

/**
 * @file const.h
 * @brief Describe custom data structures
 */

 // HTTP response structure
struct HTTP_Response
{
    char type[BUFFSIZE];		// Content-Type
    unsigned int code;			// HTTP return code
};

// Fire structure
struct File
{
    char* data;					// Pointer to data
    unsigned int length;		// Size of data
};

#endif // _TYPE_H_
