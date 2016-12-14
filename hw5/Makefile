CC=gcc
CFLAGS=-c -O2 -Wall -g

all: hw5

hw5: mem.o main.o
	$(CC) mem.o main.o -o hw5

mem.o: mem.c
	$(CC) $(CFLAGS) mem.c

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	/bin/rm -f hw5 *.o *.gz

run:
	./hw5 1000 100 200 1234

tarball:
	# put your tar command here
	tar -cvzf marshall.tar.gz *

