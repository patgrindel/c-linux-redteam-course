# Glossary

Plain-language definitions of terms used throughout the course. If a module uses a
word you don't recognize, check here.

**Address** — a number identifying a location in memory. A pointer stores one.

**Argument / parameter** — the value you pass to a function (argument) and the
variable that receives it (parameter). In C, arguments are passed **by value** (copied).

**Binary / executable** — the compiled machine-code file you run. Unlike a Python
script, it is not the source and does not need an interpreter.

**Buffer** — a block of memory (often a `char` array) used to hold data. A **buffer
overflow** is writing past its end — a classic C bug and security vulnerability.

**Compiler** — translates C source into object code. `gcc` drives the whole pipeline:
preprocessor → compiler → assembler → linker.

**Declaration vs. definition** — a *declaration* says a name exists and its type
(e.g. a function prototype in a header); a *definition* actually creates it (the
function body, or storage for a variable).

**Dereference** — follow a pointer to the thing it points at, with the `*` operator.

**Fixed-width type** — an integer type with a guaranteed size, from `<stdint.h>`:
`uint8_t`, `int32_t`, `uint64_t`, etc. Use these when size matters.

**File descriptor (fd)** — a small integer the kernel gives you to refer to an open
file/socket/pipe. `0`/`1`/`2` are stdin/stdout/stderr. Lower-level than a `FILE*`.

**Heap** — the region of memory you allocate from with `malloc` and release with
`free`. You control the lifetime.

**Header (`.h`)** — a file of declarations you `#include` so multiple `.c` files
can share the same interface.

**Linker** — combines object files and libraries into the final executable, resolving
references between them.

**Null pointer (`NULL`)** — a pointer that points to nothing. Dereferencing it
crashes (segfault). Functions often return `NULL` to signal failure.

**Object file (`.o`)** — compiled machine code for one source file, not yet linked.

**Pointer** — a variable that holds a memory address. The defining feature of C.

**Preprocessor** — runs before compilation; handles `#include`, `#define`, `#ifdef`.

**Segmentation fault (segfault)** — the OS killing your program for touching memory
it may not. Usually a bad/NULL/dangling pointer or out-of-bounds access.

**Stack** — memory for function locals and call frames, managed automatically. Fast,
but freed when the function returns — don't return pointers to locals.

**Standard library (libc)** — the C library: `printf`, `malloc`, `strlen`, etc.

**Static typing** — every variable has a type fixed at compile time (unlike Python).

**String** — in C, a `char` array terminated by a `'\0'` (NUL) byte. There is no
separate string type; the terminator is the string's only length marker.

**Struct** — a user-defined type grouping several named fields; C's closest thing to
a plain data object.

**Syscall (system call)** — a request into the OS kernel (open a file, create a
process, send on a socket). `man` section 2.

**Undefined behavior (UB)** — code the C standard gives no meaning (e.g. reading
uninitialized memory, overflow of signed ints, out-of-bounds access). May appear to
work, then fail unpredictably. Sanitizers help catch it.

**Use-after-free / double-free / dangling pointer** — using memory after you `free`d
it, freeing it twice, or keeping a pointer to freed/expired memory. All are bugs.

**Void pointer (`void*`)** — a generic pointer to any type; what `malloc` returns.
