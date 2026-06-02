#include "sout.h"
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int WidthTerminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void append(char *frame, int *pos, const char *str) {
    while (*str != '\0' && *pos < 4095) {
        frame[(*pos)++] = *str++;
    }
}

void appendInt(char *frame, int *pos, int num) {
    char buf[12];
    int i = 0;
    if (num == 0) {
        frame[(*pos)++] = '0';
        return;
    }
    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        if (*pos < 4095) frame[(*pos)++] = buf[--i];
    }
}

void redraw(char *prompt, char *buffer, int cursor, int oldCursor) {
    int terminalWidth = WidthTerminal();
    if (terminalWidth <= 0) terminalWidth = 80;

    char frame[4096];
    int framePos = 0;

    append(frame, &framePos, "\033[?25l");

    int startPosition = strlen(prompt) + oldCursor;
    int startRow = startPosition / terminalWidth;

    for (int i = 0; i < startRow; i++) {
        append(frame, &framePos, "\033[A");
    }

    append(frame, &framePos, "\r\033[J");
    append(frame, &framePos, prompt);
    append(frame, &framePos, buffer);

    int fullLength = strlen(prompt) + strlen(buffer);
    int currentPosition = strlen(prompt) + cursor;

    int totalRow = fullLength / terminalWidth;
    int cursorRow = currentPosition / terminalWidth;
    int cursorColumn = currentPosition % terminalWidth;

    for (int i = 0; i < (totalRow - cursorRow); i++) {
        append(frame, &framePos, "\033[A");
    }

    append(frame, &framePos, "\r");
    if (cursorColumn > 0) {
        append(frame, &framePos, "\033[");
        appendInt(frame, &framePos, cursorColumn);
        append(frame, &framePos, "C");
    }

    append(frame, &framePos, "\033[?25h");

    write_chunk(frame, framePos);
}
