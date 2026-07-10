
# Turgen Shell Source Tree Documentation

## 1. Introduction

Turgen Shell operates without dependency on the `ncurses` library. Everything is written from scratch especially, shell rendering and controls i/o manually **via ANSI escape codes**

And is written as 

* Minimal external dependencies.
* Portable C23-compatible.

---

## 2. Source Tree 

```txt
.
├── Makefile
└── src/
    ├── builtins/
    ├── include/
    │   ├── builtins/
    │   ├── parser/
    │   ├── proc/
    │   ├── tui/
    │   └── utils/
    ├── lib/
    ├── parser/
    ├── proc/
    ├── tui/
    ├── utils/
    └── main.c
```
### main file

`src/main.c`

Contains the primary shell loop.

(Making changes to this file in any commits is rarely permissive)

### Built-in Commands
 
`src/builtins/`

Commands implemented internally by Turgen are referred to as *"innate commands"*, *"built-ins"*, or *"standard commands"*.

### Internal Headers

`src/include/`

### Internal Libraries
`src/lib/`

### Parser
`src/parser/`

### Run Time
`src/proc/`

### Terminal User Interface
`src/tui/`

### Utilities
`src/utils/`

---

## 3. Build System

### Requirements

Turgen requires:

* GCC or another C23-compatible compiler.
* GNU/Linux or Unix-like operating environment.
* Standard glibc libraries

---

## 4. Makefile Interface

### Build Turgen

Compile the project:

```bash
make
```

### Build and Execute

Compile the shell and immediately launch it:

```bash
make run
```

### Install

Install the compiled binary into the local user binary directory:

```bash
make install
```

The installation path requires:

```text
~/.local/bin/
```

to be available through the user's `PATH` environment variable.

---

## 5. Compiler Selection

The default compiler may be overridden during compilation.

Example:

```bash
make CC=clang
```

Tugen Shell is developed  mainly with *GNU Compiler Collection*. 
But Any compiler supporting the required C23 features can be used.

---

## 6. Development Standards

### Conditional Expressions

Turgen does not permit the use of the C ternary operator.

Avoid:

```c
condition ? true_value : false_value;
```

Use explicit control flow:

```c
if (condition) {
    // true branch
} else {
    // false branch
}
```

Explicit branching is preferred because it improves readability and maintains consistency throughout the codebase.


### Nesting

Conditional nesting should not exceed three levels.

Avoid deeply nested logic:

```c
if (condition1) {
    if (condition2) {
        if (condition3) {
            if (condition4) {
                // excessive nesting
            }
        }
    }
}
```

Prefer guard-style exits:

```c
if (!condition1)
    return;

if (!condition2)
    return;

if (!condition3)
    return;
```

This keeps execution paths visible and reduces unnecessary indentation.

### Recursion

Recursive functions are **NOT** permitted.

Recursion should be used where it provides any natural solution.

### Header

The master header provides inclusions and required decleration.

```c
#include "turgen.h"
```

Individual source files should not manually include standard libraries.

Subsystem headers located under:

```text
src/include/
```

### Global Definitions

#### Shell Defines

Defined inside `turgen.h`:

```c
#define shellname "turgen"
```

This identifier represents the global name of the shell.

It is used for:

* Diagnostic messages.
* Error reporting.
* Program generated content.

#### Terminal State

Defined globally:

```c
extern struct termios Terminal;
```

This declaration exposes the active terminal configuration state.

It allows terminal-related subsystems to safely manage transitions between:

* Raw input mode.
* Normal terminal mode.

The terminal state is used by the TUI and input handling.

---
