CXX = g++
CC = gcc
CFLAGS = -Wall
TARGETS = server client
SRC_FILES = server.cpp client.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

all: $(TARGETS)

server: server.o
	$(CXX) $^ -o $@

client: client.o
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ_FILES) $(TARGETS)
