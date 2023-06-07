CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror #-pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulator/emulate.o

assemble: assemble.o
emulate: emulator/emulate.o

clean:
	$(RM) *.o assemble emulate
	
