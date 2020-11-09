CC=gcc
CFLAGS=-Wall -g

newshell: major1.c parse.h redirection.h pipelining.h
	$(CC) -o newshell $(CFLAGS) major1.c
clean:
	$(RM) newshell