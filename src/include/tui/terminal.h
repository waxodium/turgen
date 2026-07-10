#ifndef TERMINAL_H
#define TERMINAL_H

#include "turgen.h"

void enableRaw(struct termios *orgTerminal);
void disableRaw(struct termios *orgTerminal);

#endif
