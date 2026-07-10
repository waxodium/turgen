#include "turgen.h"

#include "sout.h"
#include "render.h"
#include "terminal.h"
#include "prompter.h"
#include "tree.h"

void execute(char *buffer, ShellState *state);

static char history[100][4096];
static int historyCount = 0;
static int historyView = 0;

extern struct termios Terminal;


//Continuation prompt
static int endquote(char *string) {
    char quote = '\0';
    int escape = 0;
    
    for (int i = 0; string[i]; i++) {
        if (escape) {
            escape = 0;
        } else if (string[i] == '\\') {
            escape = 1;
        } else if (string[i] == quote) {
            quote = '\0';
        } else if (quote == '\0' && (string[i] == '"' || string[i] == '\'')) {
            quote = string[i];
        }
    }
    return (quote != '\0' || escape);
}

void input(ShellState *state, char character, char *prompt) {
    state->term_width = width(); 
    int old_row = render_getrow(state, state->cursor);
    int renderAble = 0;

    switch (character) {
        
        // CTRL-A To beginning line
        case 1:
            state->cursor = 0;
            renderAble = 1;
            break;

        // CTRL-C
        case 3:
            sout("^C\r\n");
            state->buffer[0] = '\0';
            state->length = 0;
            state->cursor = 0;
            
            render_init(state, prompt);
            sout("%s", prompt);
            return;

        // CTRL-E To end of line
        case 5:
            state->cursor = state->length;
            renderAble = 1;
            break;

        // Enter key
        case 13:
            if (endquote(state->buffer)) {
                if (state->length < 4095) {
                    for (int i = state->length; i > state->cursor; i--) {
                        state->buffer[i] = state->buffer[i - 1];
                    }
                    state->buffer[state->cursor++] = '\n';
                    state->length++;
                    state->buffer[state->length] = '\0';
                    render_update(state, old_row);
                }
                return;
            }

            state->buffer[state->length] = '\0';
            sout("\r\n");
            
            if (state->length > 0) {
                if (historyCount == 100) {
                    for (int i = 0; i < 99; i++) {
                        strcpy(history[i], history[i + 1]);
                    }
                    historyCount = 99;
                }
                
                if (historyCount == 0 || strcmp(history[historyCount - 1], state->buffer) != 0) {
                    strcpy(history[historyCount], state->buffer);
                    historyCount++;
                }

                execute(state->buffer, state);
                prompter(prompt, 256);
            }
            
            historyView = historyCount;
            render_init(state, prompt);
            sout("%s", prompt);
            return;

        // BackSpace
        case 127:
            if (state->cursor > 0) {
                for (int i = state->cursor - 1; i < state->length - 1; i++) {
                    state->buffer[i] = state->buffer[i + 1];
                }
                state->cursor--;
                state->length--;
                state->buffer[state->length] = '\0';
                renderAble = 1;    
            }
            break;

        // Arrows
        case 27: {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0 || read(STDIN_FILENO, &seq[1], 1) <= 0) {
                break;
            }

            
            // Up
            if (seq[0] == '[' && seq[1] == 'A' && historyView > 0) {
                historyView--;
                strcpy(state->buffer, history[historyView]);
                state->length = strlen(state->buffer);
                state->cursor = state->length;
                renderAble = 1;
                break;
            } 
            
            // Down
            if (seq[0] == '[' && seq[1] == 'B' && historyView < historyCount) {
                historyView++;
                if (historyView == historyCount) {
                    state->buffer[0] = '\0';
                    state->length = 0;
                } else {
                    strcpy(state->buffer, history[historyView]);
                    state->length = strlen(state->buffer);
                }
                state->cursor = state->length;
                renderAble = 1;
                break;
            }

            // Right
            if (seq[0] == '[' && seq[1] == 'C' && state->cursor < state->length) {
                state->cursor++;
                renderAble = 1;
                break;
            } 
            
            // Left
            if (seq[0] == '[' && seq[1] == 'D' && state->cursor > 0) {
                state->cursor--;
                renderAble = 1;
                break;
            }

            
            // Home
            if ((seq[0] == '[' && seq[1] == 'H') || (seq[0] == 'O' && seq[1] == 'H')) {
                state->cursor = 0;
                renderAble = 1;
                break;
            }
            
            // End
            if ((seq[0] == '[' && seq[1] == 'F') || (seq[0] == 'O' && seq[1] == 'F')) {
                state->cursor = state->length;
                renderAble = 1;
                break;
            }

            // CTRL + Arrows
            if (seq[0] == '[' && seq[1] == '1') {
                char semi, mod, dir;
                if (read(STDIN_FILENO, &semi, 1) <= 0) break;
                if (read(STDIN_FILENO, &mod, 1) <= 0) break;
                if (read(STDIN_FILENO, &dir, 1) <= 0) break;

                // CTRL RIGHT
                if (semi == ';' && mod == '5' && dir == 'C') {
                    while (state->cursor < state->length && state->buffer[state->cursor] == ' ') state->cursor++;
                    while (state->cursor < state->length && state->buffer[state->cursor] != ' ') state->cursor++;
                    renderAble = 1;
                }
                
                // CTRL LEFT
                if (semi == ';' && mod == '5' && dir == 'D') {
                    while (state->cursor > 0 && state->buffer[state->cursor - 1] == ' ') state->cursor--;
                    while (state->cursor > 0 && state->buffer[state->cursor - 1] != ' ') state->cursor--;
                    renderAble = 1;
                }
            }
            break;
        }        
        // CTRL-D 
        case 4:
            sout("\r\nexit\r\n");
            disableRaw(&Terminal);
            exit(0);
            break;
        
        // TAB key
        case 9:
            if (strchr(state->buffer, '/')) {
                TabTree(state);
                return;
            }
            break;

        default:
            if (state->length < 4095 && character >= 32 && character <= 126) {
                if (state->cursor < state->length) {
                    for (int i = state->length; i > state->cursor; i--) {
                        state->buffer[i] = state->buffer[i - 1];
                    }
                }
                state->buffer[state->cursor++] = character;
                state->length++;
                state->buffer[state->length] = '\0';
                
                renderAble = 1;
            }
            break;
    }

    if (renderAble) {
        render_update(state, old_row);
    }
}
