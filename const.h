#ifndef _CONST_H_
#define _CONST_H_

/**
 * @file const.h
 * @brief Constants definition for SimpleWebServer
 */


//#define INDEX_FILE	"index.html"
#define INDEX_FILE		"country.html"
#define BUFFSIZE_DATA 	1024			// Max buffer size for data
#define BUFFSIZE_VAR	100				// Max buffer size for variables
#define REGEX_MAX_MATCH 50				// Max regular expression matching group (include
										// full match and group match)
#define MSSV 			"1412661"		// My student code

#define DEFAULT_PORT	9999
#define CONNMAX 50

#define DATABASE    "list.txt"			// File that store country and its capital

#define HTTP_200	"HTTP/1.0 200 OK\n\n"
#define HTTP_400	"HTTP/1.0 400 Bad Request\n"
#define HTTP_403	"HTTP/1.0 403 Forbidden\n"
#define HTTP_404	"HTTP/1.0 404 Not Found\n"
#define HTTP_429	"HTTP/1.0 429 Too Many Requests\n"
#define HTTP_500	"HTTP/1.0 500 Internal Server Error\n"
#define HTTP_502	"HTTP/1.0 502 Bad Gateway\n"
#define HTTP_503	"HTTP/1.0 503 Service Unavailable\n"

#endif // _CONST_H_
