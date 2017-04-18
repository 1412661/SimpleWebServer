/*
This is a very simple HTTP server. Default port is 9999

You can provide command line arguments like:- $./a.aout -p [port]

to start a server at port 50000:
$ ./webserver.out -p 50000

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
#include <fcntl.h>

#include "linklist.h"
#include "const.h"
#include "http.h"
#include "function.h"


#define CONNMAX 50
#define BYTES 1024

// watch -n 0.1 wget --delete-after http://localhost:9999

int socketfd;
void startServer(int);
void respond(int);

static volatile int keepRunning = 1;

void intHandler()
{
    printf("\nCtrl + C catched !\n");
    keepRunning = 0;

    printf("Waiting for all sockets to be closed...\n");
    int childProc =  countChildProcess(getpid());
    printf("Current child process: %d\n", childProc);
    while (childProc > 0)
    {
        childProc =  countChildProcess(getpid());
        //printf("Current child process: %d\n", childProc);
    }

    char buffer[BUFFSIZE_VAR];
    while (read(socketfd, buffer, BUFFSIZE_VAR) > 0)
    {

    }

    close(socketfd);
    printf("Socket server offline\n");
}

int main(int argc, char* argv[])
{
    signal(SIGINT, intHandler);


    int port = DEFAULT_PORT;
    if (argc == 2)
		port = atoi(argv[1]);

    startServer(port);

    // ACCEPT connections
    while (keepRunning == 1)
    {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        int clients = accept(socketfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients >= 0)
        {
            // On success, the PID of the child process is returned in the parent,
            // and 0 is returned in the child
            if (fork() == 0)
            {
                respond(clients);
                exit(0);
            }
        }
    }

    return 0;
}

/*

prevent accept() from adding more clients
have a list of the open sockets somewhere and to wait until they are all properly closed which means:
	using shutdown() to tell the client that you will no longer work on that socket
	call read() for a while to make sure that all the client has sent in the meantime has been pulled
	then using close() to free each client socket.
THEN, you can safely close() the listening socket.
*/


//start server
void startServer(int port)
{
    struct sockaddr_in serv_addr;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        printf("ERROR opening socket");

    bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding");

    if (listen(socketfd, BUFFSIZE_DATA) != 0)
		printf("ERROR on listening");

	printf("Server started at port %s%d%s width pid=%s%d%s\n", "\033[92m", port, "\033[0m", "\033[92m", getpid(), "\033[0m");
}

//client connection
void respond(int clients)
{
    char *mesg = (char*)malloc(BUFFSIZE_DATA);
    memset(mesg, 0, BUFFSIZE_DATA);

    char *returnData;
    char* requestFile;
    int bytes;
    int fileDescriptor;

    int rcvd = recv(clients, mesg, BUFFSIZE_DATA, 0);
    if (rcvd < 0)
        fprintf(stderr,("recv() error\n"));
    else if (rcvd == 0)
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else
    {
        printf("<!-- Message begin: -->\n%s<!-- Message end -->\n", mesg);

        requestFile = getRequestFile(mesg);
        printf("Requested file: |%s|\n", requestFile);
        fileDescriptor = open(requestFile, O_RDONLY);
        if (fileDescriptor != -1)    //FILE FOUND
        {
            returnData = (char*)malloc(BUFFSIZE_VAR);
            send(clients, HTTP_200, strlen(HTTP_200), 0);
            while ((bytes = read(fileDescriptor, returnData, BUFFSIZE_VAR)) > 0)
                write(clients, returnData, bytes);
        }
        else
            write(clients, HTTP_404, strlen(HTTP_404)); //FILE NOT FOUND
    }

    free(requestFile);
    shutdown(clients, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clients);
}
