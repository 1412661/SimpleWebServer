/*

This is a very simple HTTP server. Default port is 9999
You can provide command line arguments like:- $./a.aout -p [port]

to start a server at port 50000:
$ ./webserver -p 50000

http://stackoverflow.com/questions/9681531/graceful-shutdown-server-socket-in-linux
- prevent accept() from adding more clientfd
- have a list of the open sockets somewhere and to wait until they are all properly closed which means:
	+ using shutdown() to tell the client that you will no longer work on that socket
	+ call read() for a while to make sure that all the client has sent in the meantime has been pulled
	+ then using close() to free each client socket.
- THEN, you can safely close() the listening socket.

*/

// Thread & mutex
// http://www.thegeekstuff.com/2012/05/c-mutex-examples/?refcom
// http://softpixel.com/~cwright/programming/threads/threads.c.php

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include <pthread.h>
#include <semaphore.h>

#include "const.h"
#include "function.h"

// Send 10 request/s to the server
// watch -n 0.1 wget --delete-after http://localhost:9999

int parentPID;		// PID of this process
int socketfd;		// Socket File Descript that define a socket

void startServer(int port);
void respond(void*);

struct DataToPassToThread
{
    int socketfd;
    int thread_id;
};

struct TheadList
{
    int count;
    int status[CONNMAX];				// thread[i] is running if thread[i] == 1
    pthread_t handler[CONNMAX];			// Thread identifier
} thread;

pthread_mutex_t mutex;

int keepRunning = 1;
//static volatile int keepRunning = 1;

void intHandler()
{
    printf("\nCtrl + C catched !\n");
    printf("[INFO] Waiting for all sockets to be closed...\n");
    printf("[INFO] Thread left: %d\n", thread.count);
    keepRunning = 0;
    while (thread.count != 0)
    {
    }

    close(socketfd);	// accept() will return -1
    //Prevent from accepting any more connection

    printf("[WARNING] Socket server offline\n");
}

int main(int argc, char* argv[])
{
    // Warning about current working folder
    chdir(WORKING_FOLDER);
    printf("[WARNING] Working folder: %s\n", WORKING_FOLDER);
    printf("[INFO] Change working folder at const.h.\n");
    printf("[INFO] Last build: 19:15am 22.04.2017.\n");

    // Put a trap for SIGINT (Ctrl+C)
    signal(SIGINT, intHandler);

    thread.count = 0;
    memset(thread.status, 0, sizeof(thread.status));

    pthread_mutex_init(&mutex, NULL);

    int port = DEFAULT_PORT;
    if (argc == 2)
        port = atoi(argv[1]);

    startServer(port);

    while (keepRunning)
    {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
#ifdef DEBUG_MODE
        printf("[INFO] Waiting for new connection...\n");
#endif // DEBUG_MODE
        int clientfd = accept(socketfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clientfd >= 0)
        {
            printf("[INFO] Number of thread: %d\n", thread.count);
            if (thread.count == CONNMAX)
            {
                // Return code 429 when reach max number of connection
                write(clientfd, HTTP_429, strlen(HTTP_429));

                // Connection is close by these steps:
                // 1. Receive remain data from client
                // 2. Shutdown the socket
                // 3. Close the socket
                char* tmp = (char*)malloc(BUFFSIZE_DATA);
                recv(clientfd, tmp, BUFFSIZE_DATA, 0);
                free(tmp);
                shutdown(clientfd, SHUT_RDWR);
                close(clientfd);
            }
            else
            {
#ifdef DEBUG_MODE
                printf("[INFO] Connection with descriptor %d is accepted\n", clientfd);
#endif // DEBUG_MODE

                // On success, the PID of the child process is returned by fork()
                // fork() return 0 for the child
                // fork() return pid of the child for the parent

                pthread_mutex_lock(&mutex);
                int freeThreadID = findEmptyThread(thread.status);
                thread.count++;
                thread.status[freeThreadID] = 1;
                pthread_mutex_unlock(&mutex);

#ifdef DEBUG_MODE
                printf("[INFO] Thread #%d/%d is used\n", freeThreadID+1, CONNMAX);
#endif // DEBUG_MODE

                struct DataToPassToThread data;
                data.socketfd = clientfd;
                data.thread_id = freeThreadID;
                pthread_create(&(thread.handler[freeThreadID]), NULL, respond, &data);
            }
        }
    }

    return 0;
}

void startServer(int port)
{

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        printf("[ERROR] socket(): could not initialize socket");

    struct sockaddr_in serv_addr;
    //bzero((char*)&serv_addr, sizeof(serv_addr));	// Set all byte to 0
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // When port is used by another program or not release by OS,
    // switch to another port (port--)
    while (1)
    {
        serv_addr.sin_port = htons(port);
        if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            printf("[ERROR] bind(): could not bind socket at port %d\n", port--);
        else
            break;
    }

    if (listen(socketfd, BUFFSIZE_DATA) != 0)
        printf("[ERROR] listen(): could not listen socket");

    printf("[INFO] Socket server started at port %s%d%s width pid %s%d%s\n", "\033[92m", port, "\033[0m", "\033[92m", getpid(), "\033[0m");
}

//client connection
void respond(void* arg)
{
    struct DataToPassToThread* data = (struct DataToPassToThread*)arg;

    char mesg[BUFFSIZE_DATA] = "";		// set all byte to zero
    int rcvd = recv(data->socketfd, mesg, BUFFSIZE_DATA, 0);
    if (rcvd < 0)
    {
#ifdef DEBUG_MODE
        printf("[ERROR] recv() error\n");
#endif // DEBUG_MODE
    }
    else if (rcvd == 0)
    {
#ifdef DEBUG_MODE
        printf("[ERROR] Client disconnected upexpectedly.\n");
#endif // DEBUG_MODE
    }
    else
    {
        printf("-----------------------------------\n");

        char* request = extractRequest(mesg);
        printf("[INFO] Client --> \033[92mServer\033[0m: %s\n", request);

        char* file = getRequestFile(mesg);
        char *htmlFile = readFile(file);

#ifdef DEBUG_MODE
        printf("[INFO] Request file in query string: %s\n", file);
#endif // DEBUG_MODE

        if (htmlFile == NULL)
        {
            write(data->socketfd, HTTP_404, strlen(HTTP_404));
            printf("[INFO] \033[92mServer\033[0m 404 Not found.\n");
        }
        else
        {
            printf("[INFO] \033[92mServer\033[0m --> Client: %s\n", file);
            send(data->socketfd, HTTP_200, strlen(HTTP_200), 0);

            char* country = getRequestCountry(mesg);
            if (country)	// If query string contain the field "country"
            {
                char* capital = searchCap(country);
                char* returnData = (char*)malloc(BUFFSIZE_DATA);

                // Fill data into the template
                if (capital)
                    sprintf(returnData, htmlFile, "all", country, capital, "none", "");
                else
                    sprintf(returnData, htmlFile, "none", "", "", "all", country);

                // Return data to client
                write(data->socketfd, returnData, strlen(returnData));

                free(country);
                free(returnData);
            }
            else
                // Process anther request like favicon.ico, bootstrap.min.css,...
                write(data->socketfd, htmlFile, strlen(htmlFile));
        }
        free(file);
        printf("-----------------------------------\n");
    }

    // Shutdown send/receive function on the socket
    shutdown(data->socketfd, SHUT_RDWR);

    // Close and free the socket
    close(data->socketfd);

#ifdef DEBUG_MODE
    printf("[INFO] Connection at pid %d is closed\n", getpid());
#endif // DEBUG_MODE

	pthread_mutex_lock(&mutex);
	thread.count--;
	thread.status[data->thread_id] = 0;
	pthread_mutex_unlock(&mutex);
}
