CC = gcc
LIBS =
ARGS =

all:
	$(CC) $(ARGS) -c webserver.c $(LIBS)
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -c http.c $(LIBS)
	$(CC) $(ARGS) -c linklist.c $(LIBS)
	$(CC) $(ARGS) -o webserver webserver.o function.o http.o linklist.o $(LIBS)
	@rm *.o

Release:
	$(CC) $(ARGS) -c webserver.c $(LIBS)
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -c http.c $(LIBS)
	$(CC) $(ARGS) -c linklist.c $(LIBS)
	$(CC) $(ARGS) -o webserver webserver.o function.o http.o linklist.o $(LIBS)
	@rm *.o

Debug:
	$(CC) $(ARGS) -g -c webserver.c $(LIBS)
	$(CC) $(ARGS) -g -c function.c $(LIBS)
	$(CC) $(ARGS) -g -c http.c $(LIBS)
	$(CC) $(ARGS) -g -c linklist.c $(LIBS)
	$(CC) $(ARGS) -g -o webserver webserver.o function.o http.o linklist.o $(LIBS)
	@rm *.o
