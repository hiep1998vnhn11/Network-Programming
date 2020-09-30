CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: server client

server: server.o
	${CC} server.o -o server
UDP_Server.o: server.c
	${CC} ${CFLAGS} server.c

client: client.o
	${CC} client.o -o client
UDP_Client.o: client.c
	${CC} ${CFLAGS} client.c
clean:
	rm -f *.o *~ client server

