#include "sout.h"
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int WidthTerminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void redraw(char *prompt, char *buffer, int cursor, int oldCursor) {
    int terminalWidth = WidthTerminal();
    if (terminalWidth <= 0) terminalWidth = 80;

    sout("\033[?25l");

    int startPosition = strlen(prompt) + oldCursor;
    int startRow = startPosition / terminalWidth;
    
    for (int i = 0; i < startRow; i++) {
        sout("\033[A");
    }
    
    sout("\r\033[J"); 

    sout("%s%s", prompt, buffer);

    int fullLength = strlen(prompt) + strlen(buffer);
    int currentPosition = strlen(prompt) + cursor;

    int totalRow = fullLength / terminalWidth;
    int cursorRow = currentPosition / terminalWidth;
    int cursorColumn = currentPosition % terminalWidth;

    for (int i = 0; i < (totalRow - cursorRow); i++) {
        sout("\033[A");
    }

    sout("\r");
    if (cursorColumn > 0) {
        sout("\033[%dC", cursorColumn);
    }

    sout("\033[?25h");
}
