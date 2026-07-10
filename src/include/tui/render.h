#ifndef RENDER_H
#define RENDER_H

typedef struct {
    char buffer[4096];
    int length;
    int cursor;
    char prompt[256];
    int term_width;
    int last_status;
} ShellState;

void render_init(ShellState *state, const char *prompt);
void render_update(ShellState *state, int old_cursor);
int render_getrow(ShellState *state, int index);
void render_update(ShellState *state, int old_row);

int width();

void append_str(char *frame, int *pos, const char *str);
void append_int(char *frame, int *pos, int num);

#endif
