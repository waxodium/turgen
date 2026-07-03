CC     ?= gcc
CFLAGS ?= -Wall -Wextra -Werror

INCLUDES = -Ilib -Imodule -Icommand -Iui -Iinclude \
           -Iinclude/module -Iinclude/command -Iinclude/ui -Iinclude/parser

SRC    = main.c $(wildcard lib/*.c) $(wildcard module/*.c) \
         $(wildcard command/*.c) $(wildcard ui/*.c) $(wildcard parser/*.c)
BUILD  = ./build
TARGET = $(BUILD)/turgen

.PHONY: all run install clean

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET)

run: $(TARGET)
	$(TARGET)

install: $(TARGET)
	mkdir -p ~/.local/bin
	cp $(TARGET) ~/.local/bin/

clean:
	rm -rf $(BUILD)
