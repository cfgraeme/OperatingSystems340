CC=g++
CFLAGS=-c -O2 

all: cmd_pcap

cmd_pcap: hw6.o
	$(CC) hw6.o -o cmd_pcap -lpcap

hw6.o: hw6.cpp
	$(CC) $(CFLAGS) hw6.cpp

clean:
	/bin/rm -f cmd_pcap *.o

run:
	sudo ./cmd_pcap

tarball:
	 tar -cvzf marshall.tar.gz *

