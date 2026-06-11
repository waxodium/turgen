CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -Ilib -Imodule -Icommand

SRC    = main.c $(wildcard lib/*.c) $(wildcard module/*.c) $(wildcard command/*.c)
BUILD = ./build
TARGET = $(BUILD)/fash

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	$(TARGET)

clean:
	rm -r $(BUILD)

.PHONY: all run clean
