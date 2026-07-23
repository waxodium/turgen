# Coding Style

The coding conventions used throughout the Turgen Shell codebase. Following these guidelines helps maintain the coding process and its readability.

---

### Avoid the Ternary Operator

Do not use the conditional (`?:`) operator.

Avoid:

```c
condition ? value1 : value2;
```

Prefer:

```c
if (condition) {
    value = value1;
} else {
    value = value2;
}
```

---

## Conditional Nesting

Keep nesting shallow.

Avoid nesting beyond three levels. Use early returns or guard clauses to simplify control flow.

Prefer:

```c
if (!condition)
    return;

if (!ready)
    return;

/* Continue processing */
```

---

## Recursion

Recursive functions are not permitted.

Use iterative solutions instead.

---

## Header Inclusion

Every source file should include the project header.

```c
#include "turgen.h"
```

Do not include standard library headers directly unless there is a specific reason.

---

## Formatting

* Consistently use indentation throughout the project.
* Keep functions focused on a single responsibility.
* Use meaningful variable and function names.
* Remove unused variables and dead code before committing.

---

## Error Handling

Always check the return value of functions that may fail.

Handle errors immediately and report them clearly.

---
