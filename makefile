CC=gcc
CFLAGS=-I.

all: server client

server: server.o user.o admin.o book.o borrow.o
	$(CC) -o server server.o user.o admin.o book.o borrow.o

client: client.o user.o admin.o book.o borrow.o
	$(CC) -o client client.o user.o admin.o book.o borrow.o

server.o: ./entities/user.c ./entities/admin.c ./entities/book.c ./entities/borrow.c communication.h
	$(CC) -c server.c

client.o: ./entities/user.c ./entities/admin.c ./entities/book.c ./entities/borrow.c communication.h
	$(CC) -c client.c

user.o: ./entities/user.c ./entities/user.h
	$(CC) -c ./entities/user.c

admin.o: ./entities/admin.c ./entities/admin.h
	$(CC) -c ./entities/admin.c

book.o: ./entities/book.c ./entities/book.h
	$(CC) -c ./entities/book.c

borrow.o: ./entities/borrow.c ./borrow/borrow.h
	$(CC) -c ./entities/borrow.c