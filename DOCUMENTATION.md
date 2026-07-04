# Turgen Shell Documentation

Welcome to the official technical overview and user guide for **Turgen Shell**. This document helps you to understand the project layout and outlines how the shell functions.

---

## Core Architecture & Execution Loop

Turgen Shell operates as a custom terminal environment utilizing a real-time execution loop. When launched, the engine steps through the following sequence:

1. **Initialization & Terminal Configuration**:
   * Prepares internal shell tracking states.
   * Switches the terminal interface into **raw mode**, allowing it to process individual keypresses instantaneously rather than waiting for a newline delimiter.
   * Establishes custom signal handling policies (e.g., managing interruptions like `Ctrl + C`).
2. **Display Prompt**: Renders the active user interface prompt line (e.g., `user@shell:~$`).
3. **Input Interception**: Continually listens for raw keyboard input, reading characters sequentially to capture letters, Backspace events, arrow navigation, and line submissions.
4. **Parsing & Execution Dispatch**: Routes captured sequences to dedicated parsing and command execution modules.

---

## Repository Structure Overview

To help you navigate the codebase, here is a breakdown of the core directories and their responsibilities:

* **`main.c`**: The central entry point of the program that handles the main initialization and life loop.
* **`command/`**: Contains the implementations for individual built-in commands supported by the shell.
* **`parser/`**: Responsible for tokenizing and parsing user input strings into executable commands.
* **`ui/`**: Manages user interface interactions, including input prompts (`prompter.c`) and directory/history visualization structures (`tree.c`).
* **`include/`**: Houses the global header files (`.h`) containing function prototypes, macro definitions, and custom data structures.
* **`lib/`**: Contains utility libraries and standard output wrappers used across different modules (e.g., `sout.c`).

---

## Building and Running

Because this project includes a standard `Makefile`, compiling the shell locally is straightforward.

1. **Compile the source code**:
   ```bash
   make