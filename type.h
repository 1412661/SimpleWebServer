#ifndef _TYPE_H_
#define _TYPE_H_

#include "const.h"


struct ThreadList
{
    int count;
    int status[CONNMAX];				// thread[i] is running if thread[i] == 1
    pthread_t handler[CONNMAX];			// Thread identifier
    pthread_attr_t attr[CONNMAX];		// Resource manager for threads
};

struct DataToPassToThread
{
    int socketfd;
    int thread_id;
};


#endif // _TYPE_H_
