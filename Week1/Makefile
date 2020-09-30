CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: authenticate

kplc: authenticate.o
	${CC} authenticate.o -o kplc

main.o: authenticate.c
	${CC} ${CFLAGS} authenticate.c


clean:
	rm -f *.o *~ authenticate
