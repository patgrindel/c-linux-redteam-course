# Module 03 — Control Flow & Functions

*Structure programs with branches, loops, and functions; understand scope,
lifetime, `static`/`extern`, and the call stack — then return multiple values with
pointers as a bridge to Module 04.*

## Where we are

Module 02 gave you values and the operators that combine them. Now you learn to
*control* what runs and *organize* code into functions — the building blocks of every
program past "hello." Much of the syntax will feel familiar from Python; the differences
are braces instead of indentation, explicit types, and a few genuine traps (`switch`
fall-through, pass-by-value).

## Learning objectives

By the end you can:

- Branch with `if`/`else` and `switch` (and avoid the fall-through trap).
- Write `while`, `do/while`, and `for` loops and control them with `break`/`continue`.
- Declare, define, and call functions, and explain why arguments are passed **by value**.
- Reason about **scope** and **lifetime**, and use `static` and `extern` deliberately.
- Describe what the **call stack** does during (recursive) calls.
- Return multiple values from a function using **output pointers**.

## `if` / `else`

Same idea as Python, with braces for the block and parentheses around the condition.
The one conceptual difference: C has no distinct boolean in the condition — `if` just
tests whether the expression is **non-zero**. `0` is false; anything else is true.

[`examples/if_else.c`](examples/if_else.c):

```c
static char grade_for(int score)
{
    if (score >= 90) {
        return 'A';
    } else if (score >= 80) {
        return 'B';
    } else if (score >= 70) {
        return 'C';
    } else {
        return 'F';
    }
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/if_else.c -o if_else
$ ./if_else
score  95 -> grade A
score  83 -> grade B
score  72 -> grade C
score  40 -> grade F
flag is 5, which is non-zero, so this branch runs
```

**Always use braces**, even for one-line bodies. It is the house style and it prevents a
famous family of bugs where a second statement looks guarded but is not. And watch the
`=` vs `==` trap: `if (x = 5)` *assigns* 5 to `x` and is always true; `if (x == 5)`
compares. `-Wall` warns when a comparison looks accidental.

## `switch`

When you are branching on one integer or character value against many constants,
`switch` is cleaner than a long `if`/`else if` chain. The catch that bites everyone:
after jumping to a matching `case`, control runs **straight through** subsequent cases
until it hits a `break`. Forgetting `break` ("fall-through") is a classic bug — but
deliberately stacking labels is a useful idiom.

[`examples/switch_demo.c`](examples/switch_demo.c):

```c
switch (c) {
    case 'a': case 'e': case 'i': case 'o': case 'u':
        return "vowel";          /* five labels fall into one block */
    case ' ': case '\t':
        return "whitespace";
    default:
        return "other";
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/switch_demo.c -o switch_demo
$ ./switch_demo
'a' is vowel
'b' is other
' ' is whitespace
'e' is vowel
12 * 4 = 48
```

Rules to internalize: put a `break` at the end of every case unless you *mean* to fall
through; always include a `default`; and `switch` only works on integer-typed values
(`int`, `char`, `enum`), not on strings or floats. For strings you compare with
`strcmp` (Module 05).

## Loops

Three forms, one idea — repeat while a condition holds.

| Loop | Condition checked | Runs at least once? | Best for |
|---|---|---|---|
| `for` | before each pass | no | counting a known number of times |
| `while` | before each pass | no | "until some condition" |
| `do/while` | **after** each pass | **yes** | "do it, then check" (e.g. retry loops) |

[`examples/loops.c`](examples/loops.c) shows all three plus `break` (leave the loop) and
`continue` (skip to the next iteration):

```console
$ gcc -std=c11 -Wall -Wextra examples/loops.c -o loops
$ ./loops
for: sum of 1..5 = 15
while: 3 2 1 
do/while (condition false up front): ran once 
odd numbers until 7: 1 3 5 
```

The `for` header has three parts, `for (init; condition; step)`:

```c
for (int i = 1; i <= 5; i++) { sum += i; }
```

This is Python's `for i in range(1, 6)` unrolled into its mechanical parts. Note `i` is
declared *inside* the `for`, so its scope is just the loop — a C11-friendly habit that
keeps loop counters from leaking. The `do/while` line proves it runs once even though
`tries < 0` is false from the start.

## Functions

You have already written and called functions. Three things to nail down.

**Declaration vs. definition** (from Module 01): a prototype above `main` lets you call a
function defined later. **Return type** must be declared; use `void` for "returns
nothing." **Parameters have types**, listed in the parentheses.

The big one: **arguments are passed by value.** The function gets a *copy*. Assigning to
a parameter changes only the copy; the caller's variable is untouched.
[`examples/functions.c`](examples/functions.c):

```c
void try_to_change(int x)
{
    x = 999;             /* changes the local copy only */
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/functions.c -o functions
$ ./functions
max2(3, 9) = 9
before: n = 5
inside: x = 999
after:  n = 5  (unchanged - pass-by-value)
```

This surprises Python programmers, but it is subtler there too: Python passes references
to objects, so mutating a *list* inside a function is visible to the caller, while
rebinding a *name* is not. C is uniform and blunt — **everything is copied**. To let a
function modify a caller's variable, you pass the variable's *address* (a pointer); see
the last section. (`max2` also shows the ternary operator `cond ? a : b`, a compact
`if`/`else` that produces a value.)

## Scope and lifetime

Two separate questions about every variable:

- **Scope** — *where* in the source the name is visible.
- **Lifetime** — *how long* its storage (and value) exists at run time.

An ordinary local has **block scope** and **automatic lifetime**: it exists from its
declaration to the end of its enclosing `{ }`, and each function call gets a fresh one. A
**global** (declared outside any function) has file scope and lives the whole program.

The interesting case is a **`static` local**: block scope (still only visible in the
function) but *program lifetime* — it is initialized once and keeps its value between
calls. [`examples/scope_lifetime.c`](examples/scope_lifetime.c):

```c
static int next_id(void)
{
    static int id = 0;   /* initialized once; persists across calls */
    id++;
    return id;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/scope_lifetime.c -o scope_lifetime
$ ./scope_lifetime
g_config = 100
next_id() -> 1
next_id() -> 2
next_id() -> 3
inner block: x = 2
outer block: x = 1
```

`next_id` counts up across calls because its `id` is `static`. The inner `x` **shadows**
the outer one only inside its braces; outside, the outer `x` is unchanged. (Shadowing is
legal but a readability hazard — the compiler warns if you add `-Wshadow`.)

## `static` and `extern` across files

The word `static` wears two hats; both mean "limited visibility / persistent storage."

- On a **local variable**: persists across calls (just shown).
- On a **global variable or a function**: gives it **internal linkage** — it is private
  to its `.c` file. Other files cannot see it. This is how you hide a module's internal
  helpers.

Its opposite is `extern`, which **declares** a global that is **defined** in another
file, so several files can share one variable. The three-file bank module shows both.
[`examples/bank.h`](examples/bank.h) declares the shared global and the public functions;
[`examples/bank.c`](examples/bank.c) defines them plus a private `static` helper;
[`examples/bank_main.c`](examples/bank_main.c) uses them:

```c
/* bank.h */           extern int g_balance;   /* declaration: defined elsewhere */
/* bank.c */           int g_balance = 0;      /* the single definition          */
/* bank.c */           static void log_change(...) { ... }  /* file-private       */
```

```console
$ gcc -std=c11 -Wall -Wextra examples/bank_main.c examples/bank.c -o bank
$ ./bank
  [deposit 100] balance now 100
  [withdraw 30] balance now 70
  [withdraw 1000] denied, only 70 available
final balance (via extern global): 70
```

`bank_main.c` reads `g_balance` directly because the header declared it `extern`, but it
can never call `log_change` — that helper is `static`, sealed inside `bank.c`. Preferring
`static` for anything not part of a module's public interface is good hygiene (and we
rely on it heavily in the multi-file capstone). Globals themselves are best used
sparingly; they are shown here to explain the linkage keywords.

## The call stack

When a function is called, the program sets up a **stack frame** for it — a region
holding that call's parameters and local variables, plus where to return to. Calls
stack up (push); returns tear down (pop), last-in-first-out. Recursion makes this
visible: each recursive call is a new frame with its own copy of the parameters.
[`examples/call_stack.c`](examples/call_stack.c) indents by depth:

```console
$ gcc -std=c11 -Wall -Wextra examples/call_stack.c -o call_stack
$ ./call_stack
computing 4!
call factorial(4)
  call factorial(3)
    call factorial(2)
      call factorial(1)
      return 1
    return 2
  return 6
return 24
4! = 24
```

Read it top to bottom: the stack grows as each `factorial` calls a deeper one, hits the
base case, then unwinds as each frame returns and is popped. Two practical consequences:
the stack is finite, so unbounded recursion overflows it and crashes (a **stack
overflow**); and because each frame's locals vanish when it returns, you must never hand
back a pointer to a local variable — its storage is gone (a Module 04 pitfall worth
flagging now).

## Returning multiple values with pointers

A function returns one value. The C way to hand back several is to let the caller pass
the **addresses** of variables for the function to fill in — "output parameters." This is
your first real use of pointers and the motivation for the whole next module.
[`examples/multi_return.c`](examples/multi_return.c):

```c
static void divmod(int a, int b, int *quotient, int *remainder)
{
    *quotient  = a / b;   /* write through the pointer into the caller's variable */
    *remainder = a % b;
}

int main(void)
{
    int q, r;
    divmod(17, 5, &q, &r);   /* &q is "the address of q" */
    printf("17 / 5 = %d remainder %d\n", q, r);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/multi_return.c -o multi_return
$ ./multi_return
17 / 5 = 3 remainder 2
```

Two new symbols, met properly in Module 04: `&q` is "the address of `q`" (so `divmod`
knows *where* `q` lives), and `*quotient` inside the function means "the variable that
pointer points at." Writing `*quotient = ...` reaches back and changes the caller's `q`.
That is exactly how a function escapes pass-by-value — and it is the whole idea of
pointers in one example.

## Common mistakes / gotchas

- **`=` instead of `==`.** `if (x = 0)` assigns and is always false; `==` compares. Heed
  the `-Wall` warning.
- **Missing `break` in `switch`.** Control falls through to the next case. Add `break`
  unless fall-through is intentional and commented.
- **Expecting a function to change its argument.** It cannot, for a plain value — it got
  a copy. Pass a pointer to let it modify the caller's variable.
- **Off-by-one loops.** `i <= n` vs `i < n`. Decide whether the bound is inclusive and
  be consistent.
- **Returning a pointer to a local.** The local's frame is gone after the function
  returns; the pointer dangles. (More in Module 04.)
- **Unbounded recursion.** No base case, or one that never triggers, overflows the stack
  and crashes.
- **`static` confusion.** On a local it means "persist across calls"; on a global or
  function it means "private to this file." Same keyword, two uses.

## Recap

- `if`/`else` test for **non-zero**; `switch` falls through without `break` — always
  brace blocks and default your switches.
- `for`, `while`, and `do/while` differ in when the condition is checked; `break` exits a
  loop, `continue` skips an iteration.
- Functions pass arguments **by value** (a copy); to change a caller's variable, pass its
  **address**.
- **Scope** is where a name is visible; **lifetime** is how long its value lives. A
  `static` local persists across calls.
- `static` gives a global/function **internal linkage** (file-private); `extern` declares
  a global defined in another file.
- The **call stack** pushes a frame per call and pops it on return; a function's locals
  die when it returns.
- Return multiple values via **output pointers** — the gateway to Module 04.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man 3 qsort` — a standard function that takes a *function pointer*; skim it now, master
  it after Module 04.
- *The C Programming Language* (K&R), Chapter 4 — functions and program structure,
  including scope and `static`.
- `man 7 feature_test_macros` — background on how headers expose declarations (useful once
  you build multi-file projects).
