# Authoring Style Guide (for course content)

This file defines the house style so every module reads as one coherent course.

## Audience assumptions

- Reader knows Python-level programming: loops, functions, lists, dicts, basic OOP.
- Reader has **zero** C experience. Never assume a C concept the course hasn't taught yet.
- When a C idea has a Python analogue, name it, then explain where the analogy breaks.

## Module structure

Each `modules/NN-name/README.md` follows this skeleton:

1. **Title + one-line summary.**
2. **"Where we are"** — one sentence linking back to the previous module.
3. **Learning objectives** — a short bullet list ("By the end you can…").
4. **Body** — teach in small steps. Every new concept gets a *complete, compilable*
   example the reader can copy, build, and run. Show the exact `gcc` command and the
   expected output.
5. **"Common mistakes / gotchas"** — especially Python-habits that bite in C.
6. **Recap** — 3–6 bullets.
7. **Exercises** — link to `exercises/README.md`.
8. **Further reading** — relevant `man` pages and 1–2 references.

## Code style

- Compile cleanly with `gcc -std=c11 -Wall -Wextra`. No warnings in finished examples.
- Always `#include` what you use; always show a full program (with `main`) unless the
  snippet is explicitly a fragment labeled as such.
- Check return values in examples that do I/O or allocation — model good habits.
- Indent 4 spaces, K&R braces, `snake_case` for functions/variables, `UPPER_CASE` macros.
- Keep lines ≤ 100 cols. Comment the *why*, not the obvious *what*.
- Prefer `stdint.h` fixed-width types where size matters; explain the choice.

## Examples vs. exercises vs. solutions

- `examples/` — finished programs the lesson walks through. Named for the concept,
  e.g. `pointers_swap.c`.
- `exercises/` — a `README.md` with numbered tasks. Provide starter files or stubs
  where helpful (e.g. `ex1_starter.c`). Each task states the goal, constraints, and
  how to verify success.
- `solutions/` — one reference solution per exercise, named to match (`ex1.c`), each
  with a short comment block explaining the approach and any tradeoffs.

## Tone

Direct, encouraging, concrete. Short paragraphs. Prefer showing a tiny program over a
wall of prose. It's fine to say "this will segfault — run it and see."

## Security framing (Modules 09, 10, 12 especially)

Keep everything oriented to **authorized assessment of hosts you own or may test**.
The capstone is a local-host enumeration/assessment aid for OSCP study. Do not write
content whose purpose is attacking third parties, evading detection, or spreading.
Explain *why* a finding matters defensively (e.g. "a SUID binary is worth noting
because a vulnerable one can lead to privilege escalation — on a box you're assessing").
