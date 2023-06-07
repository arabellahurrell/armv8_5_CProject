CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror #-pedantic
	-o out

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulator

assemble: assemble.o
emulate: emulate.o

clean:
	$(RM) *.o assemble emulate
	
