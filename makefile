# CC=gcc
# CFLAGS=-I.

# server: 
# 	$(CC) -o server server.c ./entities/user.c ./entities/admin.c ./entities/book.c ./entities/borrow.c

# client: 
# 	$(CC) -o client client.c ./entities/user.c ./entities/admin.c ./entities/book.c ./entities/borrow.c

CC=gcc
CFLAGS=-I.
SERVER_OBJS = server.o ./entities/user.o ./entities/admin.o ./entities/book.o ./entities/borrow.o
CLIENT_OBJS = client.o ./entities/user.o ./entities/admin.o ./entities/book.o ./entities/borrow.o

all: server client

server: $(SERVER_OBJS)
	$(CC) -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CC) -o client $(CLIENT_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

./entities/%.o: ./entities/%.c ./entities/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f server client $(SERVER_OBJS) $(CLIENT_OBJS) ./entities/*.o
