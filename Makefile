CC = g++
CFLAGS = -Wall

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp -pthread
client: client.cpp
	$(CC) $(CFLAGS) -o client client.cpp -pthread

clean:
	rm -f server client

.PHONY: all clean
