#include "turgen.h"
#include "render.h"

#define RENDER_MAX_LINES 64
#define RENDER_LINE_SIZE 4096

typedef struct {
    int row;
    int column;
    int line_length;
    int cursor_row;
    int cursor_column;
} RenderCursor;

static char screen_lines[RENDER_MAX_LINES][RENDER_LINE_SIZE];
static int screen_count = 0;
static int start_column = 0;

static int cursor_column(void) {
    char buffer[32];

    if (write(STDOUT_FILENO, "\033[6n", 4) != 4) {
        return 0;
    }

    int index = 0;

    while (index < 31) {
        if (read(STDIN_FILENO, &buffer[index], 1) != 1) {
            break;
        }

        if (buffer[index] == 'R') {
            buffer[index] = '\0';
            break;
        }

        index++;
    }

    char *pointer = strchr(buffer, ';');

    if (pointer != NULL) {
        return atoi(pointer + 1) - 1;
    }

    return 0;
}

int width(void) {
    struct winsize window;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == -1 || window.ws_col == 0) {
        return 80;
    }

    return window.ws_col;
}

void append_str(char *frame_buffer, int *position, const char *string) {
    while (*string && *position < (RENDER_LINE_SIZE - 1)) {
        frame_buffer[(*position)++] = *string++;
    }
}

void append_int(char *frame_buffer, int *position, int number) {
    char temporary_buffer[12];
    int index = 0;

    if (number == 0) {
        frame_buffer[(*position)++] = '0';
        return;
    }

    while (number > 0) {
        temporary_buffer[index++] = (number % 10) + '0';
        number /= 10;
    }

    while (index > 0 && *position < (RENDER_LINE_SIZE - 1)) {
        frame_buffer[(*position)++] = temporary_buffer[--index];
    }
}

void render_init(ShellState *state, const char *prompt) {
    strncpy(state->prompt, prompt, 255);
    state->prompt[255] = '\0';

    memset(state->buffer, 0, sizeof(state->buffer));

    state->length = 0;
    state->cursor = 0;
    state->term_width = width();
    screen_count = 0;
    start_column = cursor_column();
}

static int visible_length(const char *string) {
    int length = 0;

    while (*string) {
        if (*string == '\033') {
            string++;

            if (*string == '[') {
                string++;

                while (*string && *string != 'm') {
                    string++;
                }

                if (*string) {
                    string++;
                }
            }
        } else {
            length++;
            string++;
        }
    }

    return length;
}

int render_getrow(ShellState *state, int index) {
    int prompt_length = visible_length(state->prompt);

    int row = 0;
    int column = start_column + prompt_length;

    for (int i = 0; i < index; i++) {
        if (state->buffer[i] == '\n') {
            row++;
            column = 0;
        } else {
            column++;

            if (column == state->term_width) {
                row++;
                column = 0;
            }
        }
    }

    return row;
}

static void layout_prompt(const char *text, char lines[RENDER_MAX_LINES][RENDER_LINE_SIZE], RenderCursor *cursor, int width) {
    while (*text) {
        if (*text == '\033') {
            lines[cursor->row][(cursor->line_length)++] = *text++;

            if (*text == '[') {
                lines[cursor->row][(cursor->line_length)++] = *text++;

                while (*text && *text != 'm') {
                    lines[cursor->row][(cursor->line_length)++] = *text++;
                }

                if (*text == 'm') {
                    lines[cursor->row][(cursor->line_length)++] = *text++;
                }
            }
        } else {
            lines[cursor->row][(cursor->line_length)++] = *text++;
            (cursor->column)++;

            if (cursor->column == width) {
                lines[cursor->row][cursor->line_length] = '\0';

                (cursor->row)++;
                cursor->column = 0;
                cursor->line_length = 0;

                if (cursor->row >= RENDER_MAX_LINES) {
                    break;
                }
            }
        }
    }
}

static void layout_input(ShellState *state, char lines[RENDER_MAX_LINES][RENDER_LINE_SIZE], RenderCursor *cursor) {
    for (int index = 0; index <= state->length; index++) {
        if (index == state->cursor) {
            cursor->cursor_row = cursor->row;
            cursor->cursor_column = cursor->column;
        }

        if (index == state->length) {
            break;
        }

        char character = state->buffer[index];

        if (character == '\n') {
            lines[cursor->row][cursor->line_length] = '\0';

            (cursor->row)++;
            cursor->column = 0;
            cursor->line_length = 0;

            if (cursor->row >= RENDER_MAX_LINES) {
                break;
            }
        } else {
            lines[cursor->row][(cursor->line_length)++] = character;
            (cursor->column)++;

            if (cursor->column == state->term_width) {
                lines[cursor->row][cursor->line_length] = '\0';

                (cursor->row)++;
                cursor->column = 0;
                cursor->line_length = 0;

                if (cursor->row >= RENDER_MAX_LINES) {
                    break;
                }
            }
        }
    }

    lines[cursor->row][cursor->line_length] = '\0';
}

static void draw_frame(char *frame_buffer, int *position, char current_lines[RENDER_MAX_LINES][RENDER_LINE_SIZE], int total_rows, int old_row, int cursor_row, int cursor_column) {
    append_str(frame_buffer, position, "\033[?25l");
    append_str(frame_buffer, position, "\r");

    for (int i = 0; i < old_row; i++) {
        append_str(frame_buffer, position, "\033[A");
    }

    if (start_column > 0) {
        append_str(frame_buffer, position, "\033[");
        append_int(frame_buffer, position, start_column);
        append_str(frame_buffer, position, "C");
    }

    int maximum_rows;

    if (total_rows > screen_count) {
        maximum_rows = total_rows;
    } else {
        maximum_rows = screen_count;
    }

    for (int i = 0; i < maximum_rows; i++) {
        bool match = false;

        if (i < total_rows && i < screen_count) {
            if (strcmp(current_lines[i], screen_lines[i]) == 0) {
                match = true;
            }
        }

        if (i < total_rows) {
            if (!match) {
                append_str(frame_buffer, position, current_lines[i]);
                append_str(frame_buffer, position, "\033[K");
            }

            if (i < maximum_rows - 1) {
                if (i < screen_count - 1) {
                    append_str(frame_buffer, position, "\r\033[B");
                } else {
                    append_str(frame_buffer, position, "\n\r");
                }
            }
        } else {
            append_str(frame_buffer, position, "\033[K");

            if (i < maximum_rows - 1) {
                if (i < screen_count - 1) {
                    append_str(frame_buffer, position, "\r\033[B");
                } else {
                    append_str(frame_buffer, position, "\n\r");
                }
            }
        }
    }

    screen_count = total_rows;

    for (int i = 0; i < total_rows; i++) {
        strcpy(screen_lines[i], current_lines[i]);
    }

    int rows_to_move_up = (maximum_rows - 1) - cursor_row;

    for (int i = 0; i < rows_to_move_up; i++) {
        append_str(frame_buffer, position, "\033[A");
    }

    append_str(frame_buffer, position, "\r");

    if (cursor_column > 0) {
        append_str(frame_buffer, position, "\033[");
        append_int(frame_buffer, position, cursor_column);
        append_str(frame_buffer, position, "C");
    }

    append_str(frame_buffer, position, "\033[?25h");
}

void render_update(ShellState *state, int old_row) {
    state->term_width = width();

    char frame_buffer[4096];
    int position = 0;

    char current_lines[RENDER_MAX_LINES][RENDER_LINE_SIZE];

    for (int i = 0; i < RENDER_MAX_LINES; i++) {
        current_lines[i][0] = '\0';
    }

    RenderCursor cursor = {
        .row = 0,
        .column = start_column,
        .line_length = 0,
        .cursor_row = 0,
        .cursor_column = 0
    };

    layout_prompt(state->prompt, current_lines, &cursor, state->term_width);
    layout_input(state, current_lines, &cursor);

    int total_rows = cursor.row + 1;

    draw_frame(frame_buffer, &position, current_lines, total_rows, old_row, cursor.cursor_row, cursor.cursor_column);

    write(STDOUT_FILENO, frame_buffer, position);
}
