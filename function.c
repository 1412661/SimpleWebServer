#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "const.h"
#include "type.h"
#include "function.h"

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
 * Get request file in HTTP request
 * @param HTTP request message
 * @return Requested file
 */
char* getRequestFile(char* msg)
{
    char* file;

    if ((strstr(msg, "GET / HTTP") != NULL) || strstr(msg, "HEAD / HTTP") != NULL)
    {
        file = (char*)malloc(BUFFSIZE_VAR);
        strcpy(file, INDEX_FILE);
        return file;
    }

    file = strstr(msg, "/") + 1;
    int size = 0;	// size of file string
    while (file[size] != ' ' && file[size] != '?')
        size++;

    file = clone(file, size, 1);
    file[size] = '\0';

    return file;
}

/**
 * Get country name in HTTP request
 * @param HTTP request message
 * @return Request country
 */
char* getRequestCountry(char* msg)
{
    if (!strstr(msg, "GET /result.html"))
        return NULL;

	if (strstr(msg, "country=") == NULL)
        return NULL;

    char* country = strstr(msg, "country=") + strlen("country=");

    int size = 0;	// size of country string
    while (country[size] != ' ' && country[size] != '&')
        size++;

    country = clone(country, size, 1);
    country[size] = '\0';


    char* decodedCountry = clone(country, strlen(country), 1);
    decode(country, decodedCountry);
    free(country);

    return decodedCountry;
}


/**
 * Search captital by country
 * @param country
 * @return capital of the country, NULL if couldn't find
 */
char* searchCap(char* country)
{
    FILE* f = fopen(DATABASE, "r");
    if (f == NULL)
		return NULL;

    char* capital = NULL;
    char* buff = (char*)malloc(BUFFSIZE_VAR);

    while (fgets(buff, BUFFSIZE_VAR, f) > 0)
	{
		// [Vietnam,Hanoi]
		char* bracketOpen = strstr(buff, "[");
		char* bracketClose = strstr(buff, "]");
        char* comma = strstr(buff, ",");

        if ((int)bracketClose * (int)bracketOpen * (int)comma == 0)
		{
			#ifdef DEBUG_MODE
			printf("Database is corrupted\n");
			#endif // DEBUG_MODE
			continue;
		}

        char* countryDatabase = bracketOpen+1;
        char* capitalDatabase = comma+1;
        *bracketClose = '\0';
        *comma = '\0';

        #ifdef DEBUG_MODE
        printf("Comparing %s vs %s (%s)\n", country, countryDatabase, capitalDatabase);
        #endif // DEBUG_MODE
        if (strcmp(countryDatabase, country) == 0)
		{
            capital = (char*)malloc(BUFFSIZE_VAR);
            strcpy(capital, capitalDatabase);
            break;
		}
	}

	free(buff);
    fclose(f);

    return capital;
}


 /**
 * Read file into memory
 * @param Path to file
 * @return File content (terminated by NULL), NULL if file is not exist
 */
char* readFile(char* file)
{
    FILE* f = fopen(file, "rb");
    if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	// +1 because buff is a NULL terminated string
	char* buff = (char*)malloc(size+1);

    fread(buff, size, 1, f);
    buff[size] = '\0';

    fclose(f);

    return buff;
}


/**
 * Get the first line of a HTTP request
 * @param HTTP request
 * @return First line of the HTTP request
 */
char* extractRequest(char* mesg)
{
    char* lineBreak = strstr(mesg, "\r");
    char* request = clone(mesg, lineBreak-mesg+1, 1);
    request[lineBreak-mesg+1] = '\0';

    return request;
}



/*char* char_replace(char search, char replace, char* subject)
{
    for (int i = 0; i < strlen(subject); i++)
        if (subject[i] == search)
			subject[i] = replace;

	return subject;
}*/

/*
 * Search and replace a string with another string , in a string
 * Ref: http://www.binarytides.com/str_replace-for-c/
 */
/*char *str_replace(char *search , char *replace , char *subject)
{
    char  *p = NULL , *old = NULL , *new_subject = NULL ;
    int c = 0 , search_size;

    search_size = strlen(search);

    //Count how many occurences
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        c++;
    }

    //Final size
    c = ( strlen(replace) - search_size )*c + strlen(subject);

    //New subject with new size
    new_subject = malloc( c );

    //Set it to blank
    strcpy(new_subject , "");

    //The start position
    old = subject;

    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        //move ahead and copy some text from original subject , from a certain position
        strncpy(new_subject + strlen(new_subject) , old , p - old);

        //move ahead and copy the replacement text
        strcpy(new_subject + strlen(new_subject) , replace);

        //The new start position after this search match
        old = p + search_size;
    }

    //Copy the part after the last search match
    strcpy(new_subject + strlen(new_subject) , old);

    return new_subject;
}*/

/*
int main()
{
	const char *url = "http%3A%2F%2ffoo+bar%2fabcd";
	char out[strlen(url) + 1];

	printf("length: %d\n", decode(url, 0));
	puts(decode(url, out) < 0 ? "bad string" : out);

	return 0;
}
*/

/**
 * Test if a chracter is in hex format: 0123456789ABCDEF
 * @param A character that need to be test
 * @return 1 for true, 0 for false
 */
// https://www.rosettacode.org/wiki/URL_decoding#C
inline int ishex(int x)
{
	return	(x >= '0' && x <= '9')	||
		(x >= 'a' && x <= 'f')	||
		(x >= 'A' && x <= 'F');
}

/**
 * Decode a query string in HTTP/GET request
 * Example: Vi%26%237879%3Bt+Nam will become Việt Nam
 * @param Input query string
 * @param Output query string (need to be allocated first) or NULL
 * @return A decoded query string. If the second param is NULL,
 *		   length of the decoded query string will be returned
 */
// https://www.rosettacode.org/wiki/URL_decoding#C
int decode(char *s, char *dec)
{
	char *o;
	char *end = s + strlen(s);
	int c;

	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+') c = ' ';
		else if (c == '%' && (	!ishex(*s++)	||
					!ishex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return -1;

		if (dec) *o = c;
	}

	return o - dec;
}


/**
 * Find a free thread handler in marking array
 * @param Marking array
 * @return index of the free thread, -1 if all handler is busy.
 */int findEmptyThread(int *thread)
{
    for (int i = 0; i < CONNMAX; i++)
        if (thread[i] == 0)
			return i;

	return -1;
}
