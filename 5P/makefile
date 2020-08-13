#
# Makefile for simple test of concurrent buffer
#

CC = gcc
CFLAGS = -g -Wall -std=c99 -pthread
EXES = histogram

all: $(EXES)

countChars: countChars.c concurrentBuffer.o
	$(CC) $(CFLAGS) countChars.c concurrentBuffer.o -o countChars

histogram: histogram.c concurrentBuffer.o
	$(CC) $(CFLAGS) histogram.c concurrentBuffer.o -o histogram

concurrentBuffer.o: concurrentBuffer.c
	$(CC) $(CFLAGS) -c concurrentBuffer.c

clean:
	-rm -f *.o $(EXES)

