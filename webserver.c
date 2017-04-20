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

// Send 10 request/s to the server
// watch -n 0.1 wget --delete-after http://localhost:9999

int parentPID;
int socketfd;
void startServer(int);
void respond(int,int);

static volatile int keepRunning = 1;

void intHandler()
{
	if (getpid() != parentPID)
		return;

    printf("\nCtrl + C catched !\n");
    printf("Waiting for all sockets to be closed...\n");

    int childProc =  countChildProcess(getpid());
    printf("Current child process: %d\n", childProc);
    while (childProc > 0)
    {
        childProc =  countChildProcess(getpid());
        //printf("Current child process: %d\n", childProc);
    }

    close(socketfd);	// accept() will return -1
    printf("Socket server offline\n");

	//Prevent from accpenting any more connection
    keepRunning = 0;
}

int main(int argc, char* argv[])
{
	// Get PID of parent process
	parentPID = getpid();

    signal(SIGINT, intHandler);

    int port = DEFAULT_PORT;
    if (argc == 2)
		port = atoi(argv[1]);

    startServer(port);

    while (keepRunning == 1)
    {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        printf("Waiting for new connection...\n");
        int clientfd = accept(socketfd, (struct sockaddr *) &clientaddr, &addrlen);

        int childProc =  countChildProcess(getpid());
        if (clientfd >= 0)
        {
        	//printf("Connection with descriptor %d is accepted\n", clientfd);
            // On success, the PID of the child process is returned in the parent,
            // and 0 is returned in the child
            // at parent process, fork() == 0 return 0
            // at child process, fork() == 0 return 1
            if (fork() == 0)
            {
                respond(clientfd, childProc);
                break;
            }
        }
    }

    return 0;
}

void startServer(int port)
{
    struct sockaddr_in serv_addr;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        error("socket(): could not initialize socket");

    bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // When port is used by another program or not release by OS,
    // switch to another port (port--)
    while (1)
	{
		serv_addr.sin_port = htons(port);
		if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			printf("bind(): could not bind socket at port %d\n", port--);
        else
			break;
	}

    if (listen(socketfd, BUFFSIZE_DATA) != 0)
		error("listen(): could not listen socket");

	printf("Socket server started at port %s%d%s width pid %s%d%s\n", "\033[92m", port, "\033[0m", "\033[92m", getpid(), "\033[0m");
}

//client connection
void respond(int clientfd, int connections)
{
	if (connections > CONNMAX)
	{
		write(clientfd, HTTP_429, strlen(HTTP_429));
		close(clientfd);
		return;
	}

    char mesg[BUFFSIZE_DATA] = "";
    char returnData[BUFFSIZE_DATA] = "";

    char* file;
    char* country;
    int f, bytes;

    int rcvd = recv(clientfd, mesg, BUFFSIZE_DATA, 0);
    if (rcvd < 0)
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0)
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    else
    {
        printf("<!-- Message begin: -->\n%s<!-- Message end -->\n", mesg);

		file = getRequestFile(mesg);
		country = getRequestCountry(mesg);

		printf("Requested file: |%s|\n", file);
		printf("Requested country: |%s|\n", country);

		if ((f = open(file, O_RDONLY)) == -1)
			write(clientfd, HTTP_404, strlen(HTTP_404));
		else
		{
			send(clientfd, HTTP_200, strlen(HTTP_200), 0);
			while ((bytes = read(f, returnData, BUFFSIZE_DATA)) > 0)
			{
				//printf("Byte wrote: %d\n", write(clientfd, returnData, bytes));
				write(clientfd, returnData, bytes);
			}
		}

    }

    free(file);
    shutdown(clientfd, SHUT_RDWR);         // All further send and recieve operations are DISABLED...
    close(clientfd);

    //printf("Current connection is closed\n");
}
