# tiny

A minimal mini-C-style compiler built for compiler backend and toolchain
research. tiny compiles `.tiny` source through a full self-contained pipeline —
all the way down to in-memory machine code execution, with no external linker
and no intermediate files on disk.

## What tiny does

tiny takes a `.tiny` source file, compiles it to LLVM IR, lowers it to an
in-memory COFF object, links and loads it entirely in memory, and executes it
directly. Nothing is written to disk; the program's only observable output is
its runtime effect (e.g. `print_int`).

```
source.tiny
  -> Scanner        (tokens)
  -> Parser         (syntax tree)
  -> NodeBuilder    (typed AST)
  -> Sema           (name resolution, scopes, type checks, annotations)
  -> CodeGen        (LLVM IR)
  -> Optimizer      (optional custom pass pipeline)
  -> Linker         (LLVM IR -> in-memory COFF -> parse -> relocate)
  -> Loader         (VirtualAlloc -> copy -> patch GOT -> protect RX -> jump)
  -> run            (executes in-process)
```

## Language features

### Types

- `int` (32-bit signed)
- `int[]` (one-dimensional array; backed by a raw heap pointer, no length or
  bounds checking)

### Functions

- Function definitions with parameters and return values
- Recursion and forward references (two-pass semantic analysis)

### Variables

- Local variable declarations: `int x;` / `int x = expr;`
- Assignment statements: `x = expr;` and `a[i] = expr;`
- Block-level lexical scoping (same name allowed in different blocks)

### Control flow

- `if` / `if ... else` (dangling-else resolved via the grammar's
  open/close-statement technique)
- `while` loops

### Operators

- Arithmetic: `+` `-` `*` `/` `%` (correct precedence, parentheses)
- Comparison: `<` `>` `<=` `>=` `==` `!=`

### Arrays

- Allocation: `int[] a = new int[n];`
- Element read/write: `a[i]`, `a[i] = expr;`

### Built-in (native) functions

- `print_int(int)` — prints an integer followed by a newline

## Backend: link and execute

Unlike a typical compiler that emits an object file and shells out to a system
linker, tiny does the linking and loading itself, in memory:

1. **CodeGen** produces an `llvm::Module`.
2. **Optimizer** (optional) runs tiny's own custom LLVM pass pipeline over the
   module — a research playground for backend optimization.
3. **Linker** uses LLVM's `TargetMachine` to emit a COFF object into a memory
   buffer (never to disk), then parses that COFF with LLVM's object library,
   stages the `.text` / `.data` sections, builds GOT slots for undefined
   (native) symbols, and applies `IMAGE_REL_AMD64_REL32` relocations by hand.
   The result is an in-memory `LinkedImage` (code bytes, data bytes, imports,
   symbol offsets) — no on-disk object format.
4. **Loader** allocates memory with `VirtualAlloc`, copies the code and data,
   patches the GOT slots with the real addresses of native functions
   (e.g. `print_int`, `malloc`), marks the code region executable with
   `VirtualProtect` (`PAGE_EXECUTE_READ`), looks up `main`, and jumps into it.

Native calls go through a GOT slot: the IR loads a function pointer from a
global slot and calls it indirectly, and the loader fills that slot at load
time. This mirrors the core idea of a PLT/GOT without depending on a real
linker.

## Example

```c
int main() {
    int sum = 0;
    int i = 1;
    while (i < 11) {
        sum = sum + i;
        i = i + 1;
    }
    print_int(sum);
    return 0;
}
```

Run it:

```
> tiny.exe loop.tiny
55
```

Recursion:

```c
int fib(int n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    print_int(fib(10));
    return 0;
}
```

```
> tiny.exe fib.tiny
55
```

## Usage

```
tiny.exe [-emit-llvm] [-O] <file.tiny>
```

- `<file.tiny>` — compile and run a tiny program
- `-emit-llvm` — print the generated LLVM IR
- `-O` — run tiny's custom optimization pass pipeline before linking

A second tool, `dump.exe <file.tiny>`, dumps the tokens, parse tree, and
annotated AST for debugging the front end.

## Requirements

- CMake 3.20+
- A C++20 compiler
- LLVM 18
- csio library

## Building

```
cmake -B build
cmake --build build --config Release
```