# ShellState

`ShellState` is a type definition. It provides shared information required by shell subsystems, including input handling, terminal rendering, and command execution.

This, stores the active state of a Turgen shell session.

(you will see this type being used often across the code)

---

## Definition

```c
typedef struct {
    char buffer[4096];
    int length;
    int cursor;
    char prompt[256];
    int term_width;
    int last_status;
} ShellState;
```

---

## Members

### `buffer`

Stores the current command-line input.

The input handler writes user input into this buffer, while the parser reads it when processing commands.

The buffer has a fixed size of 4096 bytes.

---

### `length`

Stores the current number of characters in the input buffer.

Used to track the active portion of `buffer` during editing and parsing.

---

### `cursor`

Stores the current cursor position within the input buffer.

Used by the terminal interface for interactive line editing.

---

### `prompt`

Stores the active shell prompt string.

The rendering system displays this value before user input.

Example:

```c
state->prompt = "turgen> ";
```

---

### `term_width`

Stores the current width of the terminal.

Used by the terminal interface for calculating screen layout and rendering.

---

### `last_status`

Stores the exit status of the most recent shell operation.

A value of `0` indicates success. Non-zero values represent errors or failed operations.

Example:

```c
state->last_status = 2;
```

Syntax validation failures and command errors update this value.

---

## Usage

`ShellState` is passed between shell components that require access to the current session state.

It is for:

* Interactive input.
* Terminal rendering.
* Command execution status.

