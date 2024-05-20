CPP=g++
C=gcc
CPPFLAGS=-g -Wall -Wextra -std=c++17 -Wno-error=unused-variable
CFLAGS=-g -Wall -Wextra -Wno-error=unused-variable
LDFLAGS=

PROJECT=client

CPPSRCS=client.cpp lib/utils.cpp lib/http-utils.cpp
CSRCS=lib/buffer.c


CPPOBJS=$(CPPSRCS:.cpp=.o)
COBJS=$(CSRCS:.c=.o)

all: $(PROJECT)

build: client

client: $(CPPOBJS) $(COBJS)
	$(CPP) $(CPPOBJS) $(COBJS) -o $@

.cpp.o:
	$(CPP) -c $(CPPFLAGS) -fPIC $< -o $@
.c.o:
	$(C) -c $(CFLAGS) -fPIC $< -o $@

run:
	./client



clean:
	rm -rf *.o lib/*.o client
