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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include "const.h"
#include "function.h"

// Send 10 request/s to the server
// watch -n 0.1 wget --delete-after http://localhost:9999

int parentPID;
int socketfd;
void startServer(int);
void respond(int);

static volatile int keepRunning = 1;

void intHandler()
{
    if (getpid() != parentPID)
        return;

    printf("\nCtrl + C catched !\n");
    printf("[INFO] Waiting for all sockets to be closed...\n");

    int childProc =  countChildProcess(getpid());
    printf("[INFO] Current child process: %d\n", childProc);
    while (childProc > 0)
    {
        childProc =  countChildProcess(getpid());
#ifdef DEBUG_MODE
        printf("[INFO] Current child process: %d\n", childProc);
#endif // DEBUG_MODE
    }

    close(socketfd);	// accept() will return -1
    printf("[WARNING] Socket server offline\n");

    //Prevent from accpenting any more connection
    keepRunning = 0;
}

int main(int argc, char* argv[])
{
    // Get PID of parent process
    parentPID = getpid();

    // Warning about current working folder
    chdir(WORKING_FOLDER);
    printf("[WARNING] Working folder: %s\n", WORKING_FOLDER);
    printf("[INFO] Change working folder at const.h.\n");
    printf("[INFO] Last build: 00:05am 22.04.2017.\n");

    signal(SIGINT, intHandler);

    int port = DEFAULT_PORT;
    if (argc == 2)
        port = atoi(argv[1]);

    startServer(port);

    while (keepRunning == 1)
    {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
#ifdef DEBUG_MODE
        printf("[INFO] Waiting for new connection...\n");
#endif // DEBUG_MODE
        int clientfd = accept(socketfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clientfd >= 0)
        {
            if (countChildProcess(getpid()) == CONNMAX)
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
                if (fork() == 0)
                {
#ifdef DEBUG_MODE
                    printf("[INFO] Connection at pid %d is opened\n", getpid());
#endif // DEBUG_MODE
                    respond(clientfd);
                    break;
                }
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
void respond(int clientfd)
{
    char mesg[BUFFSIZE_DATA] = "";		// set all byte to zero
    int rcvd = recv(clientfd, mesg, BUFFSIZE_DATA, 0);
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
            write(clientfd, HTTP_404, strlen(HTTP_404));
            printf("[INFO] \033[92mServer\033[0m 404 Not found.\n");
        }
        else
        {
            printf("[INFO] \033[92mServer\033[0m --> Client: %s\n", file);
            send(clientfd, HTTP_200, strlen(HTTP_200), 0);

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
                write(clientfd, returnData, strlen(returnData));

                free(country);
                free(returnData);
            }
            else
                // Process anther request like favicon.ico, bootstrap.min.css,...
                write(clientfd, htmlFile, strlen(htmlFile));
        }
        free(file);
        printf("-----------------------------------\n");
    }

    // Shutdown send/receive function on the socket
    shutdown(clientfd, SHUT_RDWR);

    // Close and free the socket
    close(clientfd);

#ifdef DEBUG_MODE
    printf("[INFO] Connection at pid %d is closed\n", getpid());
#endif // DEBUG_MODE
}
