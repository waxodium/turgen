#include "turgen.h"
#include "terminal.h"
#include "render.h"

int run_command(char **argv, int argc, ShellState *state);

void seperate(char **argv, int argc, ShellState *state) {
    int cursor = 0;

    while (cursor < argc) {
        char *args[1024];
        int count = 0;

        while (cursor < argc && strcmp(argv[cursor], ";") != 0) {
            args[count++] = argv[cursor++];
        }
        
        if (cursor < argc && strcmp(argv[cursor], ";") == 0) {
            cursor++;
        }

        args[count] = NULL;
        if (count == 0) continue;

        run_command(args, count, state);
    }
}
