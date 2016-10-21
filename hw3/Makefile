CC=gcc
CFLAGS=-c -Wall -g

all: hw3

hw3: web.o hw3.o
	$(CC) web.o hw3.o -o hw3 -lpthread

web.o: web.c
	$(CC) $(CFLAGS) web.c

hw3.o: hw3.c
	$(CC) $(CFLAGS) hw3.c

clean:
	/bin/rm -f hw3 *.o *.gz

run:
	./hw3 url.txt

tarball:
	tar -cvzf <your last name>.tar.gz *

