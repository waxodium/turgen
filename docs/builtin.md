# Built-in Registration

Built-in commands, referred to by Turgen Shell as innates or standard commands, are implemented as C functions and registered in a static command table.

During command execution, the shell searches the innate registry before resolving external programs. A matching innate is executed immediately. External programs sharing the same name are disregarded.

---

## Command Interface

Every built-in must implement the following function signature:

```c
int command(char **argv, ShellState *state);
```

Parameters:

* `argv` — NULL-terminated argument vector.
* `state` — Pointer to the active shell state.

The function returns an integer status code.

Example:

```c
int echo(char **argv, ShellState *state)
{
    (void) state;

    /* Codes.. */

    return 0;
}
```

---

## Registration Table

Built-ins are registered in a static lookup table.

```c
typedef int (*Handler)(char **, ShellState *);

typedef struct {
    const char *name;
    Handler func;
} innate;

static innate builtins[] = {
    {"cls",  tclear},
    {"exit", texit},
    {"cd",   cd},
    {"echo", echo},
};
```

Each entry maps a command name to its implementation.

---

## Adding a Built-in

To register a new built-in:

1. Create the command implementation.
2. Declare the function in its header.
3. Include the header in the registry.
4. Add a new entry to the `builtins[]` table.

Example:

```c
#include "hello.h"

static innate builtins[] = {

    {"hello", hello},

};
```

Once registered, the command is available without requiring any additional configuration.

---

## Command Lookup

When a command is entered:

1. The parser extracts the command name.
2. The registry is searched sequentially.
3. If a matching entry is found, its handler is invoked.
4. If no match exists, the command is executed as an external program.

---


