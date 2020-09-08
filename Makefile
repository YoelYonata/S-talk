
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: build

build:
	gcc ${CFLAGS} main.c receiver.c input.c transmitter.c output.c instructorList.o -lpthread -o s-talk

clean: 
	rm -f s-talk
