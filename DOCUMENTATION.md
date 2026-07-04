# Turgen Shell

Turgen Shell is a base shell that is not written BY STANDARD from any notable **shell family**, it is also a non **POSIX-compliant** shell. Is written with C, strictly without the uses of *ncurses* library.

For formal reference, Turgen Shell is **POSIX-independent**

## Repository Tree

```txt
.
├── command/
├── include/
│   ├── command/
│   ├── module/
│   ├── parser/
│   ├── ui/
│   └── turgen.h
├── lib/
├── module/
├── parser/
├── ui/
└── main.c
```

- `command/`: Contains built-in shell commands.
- `include/`: Header files for various components, organized into subdirectories:
    - `command/`: Headers for shell builtins.
    - `module/`: Headers for shell behavior modules.
    - `parser/`: Headers for the shell parser.
    - `ui/`: Headers for the user interface.
    - `turgen.h`: Main shell-wide header.
- `lib/`: External libraries integrated into the shell.
- `module/`: Core modules defining shell behavior and functionality.
- `parser/`: Components responsible for parsing shell input.
- `ui/`: User interface components responsible for shell display and interaction.
- `main.c`: Entry point of the shell application.


## Turgen Shell's Compiler
Turgen Shell is natively developed and maintained using [GCC (The GNU Compiler Collection)](https://en.wikipedia.org/wiki/GNU_Compiler_Collection). GCC represents the baseline development environment.

**Alternative Compiler** 
- **Turgen** Makefile has override capabilitty for compilers option via standard Makefile flags

### Use differnt compilers
To substitute GCC, you must explicitly define the CC variable during compilation. Take an example the compilation with *clang*:

```bash
make CC=clang
```

**make options**
```bash 
# compile then run compiled binary
make run

# compile binary
make build

# install to ~/.local/bin/
make install
```

Installation with turgen's `make install` may require to export `~/.local/bin/` to your PATH. Any commitment of CHANGES to Makefile options will profoundly reject.

# How to code for Turgen Shell

## Enviroment
Turgen Shell development enviroment uses [C23](https://en.wikipedia.org/wiki/C23_(C_standard_revision) which is the standard C revision

The **Turgen shell** codebase is designed to be *architecture-neutral*.
For platform **GNU/Linux & Unix**


## Prohibition of Ternary

Instead of using the C conditional ternary statement syntax:
```c
condition ? value_if_true : value_if_false
```

Turgen Shell, prefer the conditional statement with if-else standard syntax instead: 
```c
if (condition) {
  ;
} else if (condition) {
  ;
} else {
  ;
}
```

## Nesting
Turgen Shell does not strictly prohibit if-else nesting:

Limit if-else nesting to a maximum of 3 layers:
```c
if (condition1) {
    if (condition2) {
        if (condition3) {

        }
    }
}
```
Use guard clauses to reduce if-else nesting beyond this limit.

```c
if (!condition1) return;
if (!condition2) return;
if (!condition3) return;
if (!condition4) return;
```

## Recursions
There are no limits of using recursions. As long as it is necesarry for a self-calling function for that returns.

## Includes
Turgen Shell codes uses the **Main shell-wide header** file named `turgen.h`.

All of `turgen.h` content are standard libraries declaration with the main definition of `shellname` and extern of `stuct termios` named `Terminal` that is used wide across the repository:

```c
#ifndef TURGEN
#define TURGEN

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <pwd.h>
#include <poll.h>
#include <libgen.h>
#include <dirent.h>
#include <glob.h>
#include <fcntl.h>
#include <ctype.h>


#define shellname "turgen"

extern struct termios Terminal;

#endif
```

Turgen Shell **DOES NOT** declare standard libraries manually on every single source code file. But local header files manual inclusions for source code is encouraged.

---
