CC = gcc
LIBS = -lpthread

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# Can use #pragma GCC diagnostic ignored in code
# #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
# #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

IGNORE = -Wno-incompatible-pointer-types -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
ARGS =

all:
	$(CC) $(IGNORE) $(ARGS) -c webserver.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -c function.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -o webserver webserver.o function.o $(LIBS)
	@rm *.o

Release:
	$(CC) $(IGNORE) $(ARGS) -c webserver.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -c function.c $(LIBS)
	$(CC) $(IGNORE) $(ARGS) -o webserver webserver.o function.o $(LIBS)
	@rm *.o

Debug:
	$(CC) $(ARGS) -g -c webserver.c $(LIBS)
	$(CC) $(ARGS) -g -c function.c $(LIBS)
	$(CC) $(ARGS) -g -o webserver webserver.o function.o $(LIBS)
	@rm *.o

cleanDebug:
	sh/clean.sh

cleanRelease:
	sh/clean.sh
