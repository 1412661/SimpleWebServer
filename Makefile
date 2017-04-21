CC = gcc
LIBS =
ARGS =

all:
	$(CC) $(ARGS) -c webserver.c $(LIBS)
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -o webserver webserver.o function.o $(LIBS)
	@rm *.o

Release:
	$(CC) $(ARGS) -c webserver.c $(LIBS)
	$(CC) $(ARGS) -c function.c $(LIBS)
	$(CC) $(ARGS) -o webserver webserver.o function.o $(LIBS)
	@rm *.o

Debug:
	$(CC) $(ARGS) -g -c webserver.c $(LIBS)
	$(CC) $(ARGS) -g -c function.c $(LIBS)
	$(CC) $(ARGS) -g -o webserver webserver.o function.o $(LIBS)
	@rm *.o
