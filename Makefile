CC     ?= gcc
CFLAGS ?= -Wall -Wextra -Werror

INCLUDES = -Isrc \
           -Isrc/lib \
           -Isrc/include \
           -Isrc/include/builtins \
           -Isrc/include/parser \
           -Isrc/include/proc \
           -Isrc/include/tui \
           -Isrc/include/utils

SRC    = src/main.c \
         $(wildcard src/builtins/*.c) \
         $(wildcard src/lib/*.c) \
         $(wildcard src/parser/*.c) \
         $(wildcard src/proc/*.c) \
         $(wildcard src/tui/*.c) \
         $(wildcard src/utils/*.c)

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
