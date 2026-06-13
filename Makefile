CC := gcc
CFLAGS := -Wall -Wextra -g

SRC := src
OUT := output

TARGET := fbc

all:
	$(CC) $(CFLAGS) $(SRC)/compiler.c -o $(TARGET)

clean:
	rm -rf $(TARGET) $(OUT)/*.o $(OUT)/*.asm

test:
	./$(TARGET) tests/main.bikt
