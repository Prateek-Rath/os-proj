CC=gcc
CFLAGS=-I.

all: server client

server: server.o user.o admin.o book.o borrow.o
	$(CC) -o server server.o user.o admin.o book.o borrow.o -pthread

client: client.o user.o admin.o book.o borrow.o
	$(CC) -o client client.o user.o admin.o book.o borrow.o

server.o: server.c communication.h
	$(CC) $(CFLAGS) -c server.c -pthread

client.o: client.c communication.h
	$(CC) $(CFLAGS) -c client.c

user.o: ./entities/user.c ./entities/user.h
	$(CC) $(CFLAGS) -c ./entities/user.c

admin.o: ./entities/admin.c ./entities/admin.h
	$(CC) $(CFLAGS) -c ./entities/admin.c

book.o: ./entities/book.c ./entities/book.h
	$(CC) $(CFLAGS) -c ./entities/book.c

borrow.o: ./entities/borrow.c ./entities/borrow.h
	$(CC) $(CFLAGS) -c ./entities/borrow.c

clean:
	rm -f server client *.o ./entities/*.o
