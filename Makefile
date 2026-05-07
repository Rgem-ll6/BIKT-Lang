C := gcc
CFLAGS := -Wall -Wextra
all: compiler
compiler: compiler.c
	$(C) $(CFLAGS) -o fbc compiler.c
clean: 
	rm -f ./fbc
run: compiler
	./fbc
.PHONY: all clean run