## This is a simple Makefile with lots of comments
## Check Unix Programming Tools handout for more info.

# Define what compiler to use and the flags.
CC=cc
CXX=CC
CCFLAGS= -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Werror -Wextra -Wconversion -DCHICKEN

all: shell errorExplain subprocess

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)

%.o : %.c
	$(CC) -c $(CCFLAGS) $<



shell: shell.o errorExplain.o aux.o subprocess.o
	$(CC) -o shell *.o -lreadline $(CCFLAGS)

clean:
	rm -f core *.o shell shell_sol
