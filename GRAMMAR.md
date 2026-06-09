# tiny Language Grammar


## Lexical structure

### Whitespace and comments (skipped)

- Whitespace: spaces, tabs, and line terminators (`\n`, `\r`, `\r\n`).
- Line comment: `// ...` to end of line.
- Block comment: `/* ... */` (does not nest).

### Keywords

```
int   if   else   while   new   return
```

### Identifiers

Start with a letter or underscore, followed by any number of letters, digits,
or underscores:

```
[A-Za-z_][A-Za-z0-9_]*
```

(A keyword like `int` is always recognized as that keyword, never as an
identifier.)

### Integer literals

A sequence of decimal digits:

```
[0-9]+
```

### Operators and punctuation

```
+   -   *   /   %          arithmetic
<   >   <=  >=  ==  !=      comparison
&&  ||  !                  logical
=                          assignment
;   ,                      separators
( )   { }   [ ]            parentheses / braces / brackets
```

---

## Program structure

A program is one or more function definitions:

```
program        ::=  function-definition+
```

### Function definitions

A function has a return type, a name, a parameter list (possibly empty), and a
block body:

```
function       ::=  type IDENTIFIER "(" parameters? ")" block

parameters     ::=  parameter ("," parameter)*
parameter      ::=  type IDENTIFIER
```

Example:

```c
int add(int a, int b) {
    return a + b;
}

int main() {
    print_int(add(2, 3));
    return 0;
}
```

### Types

```
type           ::=  "int"          // 32-bit signed integer
                 |  "int" "[" "]"  // one-dimensional int array
```

---

## Statements

A block is a brace-enclosed sequence of statements (possibly empty):

```
block          ::=  "{" statement* "}"
```

The available statements:

```
statement      ::=  if-statement
                 |  while-statement
                 |  return-statement
                 |  local-var-decl
                 |  assignment
                 |  expression ";"
                 |  block
```

### if / if-else

```
if-statement   ::=  "if" "(" expression ")" statement
                 |  "if" "(" expression ")" statement "else" statement
```

The classic "dangling else" is resolved the usual way: an `else` binds to the
nearest preceding `if` that doesn't already have one. (Internally the grammar
splits statements into "open" and "closed" forms to express this without
ambiguity; that distinction is invisible in the source language.)

### while

```
while-statement ::=  "while" "(" expression ")" statement
```

There is no `break` or `continue`.

### return

```
return-statement ::=  "return" ";"
                   |  "return" expression ";"
```

### Local variable declarations

```
local-var-decl ::=  type IDENTIFIER ";"
                 |  type IDENTIFIER "=" expression ";"
```

Variables are block-scoped; the same name may be reused in a different block.
A declaration's initializer cannot refer to the variable being declared.

### Assignment

The left-hand side is an *lvalue* — either a plain variable or an array element:

```
assignment     ::=  lvalue "=" expression ";"

lvalue         ::=  IDENTIFIER                       // x = ...
                 |  IDENTIFIER "[" expression "]"    // a[i] = ...
```

---

## Expressions

Expressions are layered to encode operator precedence. From lowest precedence
(loosest binding) to highest:

```
expression     ::=  logicalOr

logicalOr      ::=  logicalAnd
                 |  logicalOr "||" logicalAnd

logicalAnd     ::=  equality
                 |  logicalAnd "&&" equality

equality       ::=  relational
                 |  equality ("==" | "!=") relational

relational     ::=  additive
                 |  relational ("<" | ">" | "<=" | ">=") additive

additive       ::=  multiplicative
                 |  additive ("+" | "-") multiplicative

multiplicative ::=  primary
                 |  multiplicative ("*" | "/" | "%") primary
```

All binary operators are **left-associative**. From lowest to highest
precedence:

- `||`
- `&&`
- `== !=`
- `< > <= >=`
- `+ -`
- `* / %`
- `!` (unary prefix, binds tightest)

### Primary expressions

```
primary        ::=  INTEGER                          // 42
                 |  IDENTIFIER                        // x
                 |  "(" expression ")"                // grouping
                 |  call                              // f(...)
                 |  "new" "int" "[" expression "]"    // allocate int array
                 |  IDENTIFIER "[" expression "]"     // array element read
                 |  "!" primary                       // logical not
```

### Function calls

```
call           ::=  IDENTIFIER "(" arguments? ")"

arguments      ::=  expression ("," expression)*
```

A call may target a user-defined function or a built-in native function (e.g.
`print_int`).

---

## Notes / limitations

- Only one integer type (`int`) and a one-dimensional `int[]` array type.
- Arrays are raw pointers: `new int[n]` allocates `n` ints, with **no length
  field and no bounds checking**.
- Comparison results are boolean only — usable in `if` / `while` conditions,
  but not storable into an `int`.
- No `break` / `continue`, no `for`, no bitwise operators, no unary minus.