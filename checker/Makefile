CC=g++
CFLAGS=-c -g -Wall -Wextra -std=c++17 -Wno-error=unused-variable
LDFLAGS=

PROJECT=client

SRCS=client.cpp lib/utils.cpp lib/http-utils.cpp

#SRCS=client.cpp
OBJS=$(SRCS:.cpp=.o)

all: $(PROJECT)

build: client

client: $(OBJS)
	$(CC) $(OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -fPIC $< -o $@
.c.o:
	$(CC) $(CFLAGS) -fPIC $< -o $@

run:
	./client

clean:
	rm -rf *.o client $(OBJS)
