CC=gcc
CFLAGS=-g -Wall
SRC=Shell.c lex.yy.c

all:
	flex Shell_lex.l
	$(CC) $(CFLAGS) -o shell $(SRC) -lfl -lreadline -ltermcap

clean:
	rm -f  shell.o lex.yy.c
