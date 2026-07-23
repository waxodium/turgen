# Compiler Selection

Tugen Shell uses **GNU Compiler Collection (GCC)** as its primary compiler for development and testing. However, any compiler with the required **C23** language support is compatible.

## Default Compiler

By default, the build system uses the compiler configured in the project's `Makefile`.

## Selecting a Different Compiler

Override the compiler by setting the `CC` variable when running `make`.

```bash
make CC=clang
```

Other examples:

```bash
make CC=gcc
make CC=zig cc
```

## Requirements

* Support for the **C23** standard.
* Compatibility with the project's build options and dependencies.
