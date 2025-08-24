# The DIM Programming Language

## Scopes

For now, without functions being implemented yet, your program MUST be contained in a global scope.

```
{
    // Your code here
};
```

## Datatypes

### Null

Null values are implemented, can be used with the `null` keyword.
However, for now, they don't have a use, neither a datatype, so they cannot be assigned.

### Number

Numbers are divided in the following types :
```
{
    const valI8: i8 = 0;
    const valI16: i16 = 0;
    const valI32: i32 = 0;
    const valI64: i64 = 0;

    const valU8: u8 = 0;
    const valU16: u16 = 0;
    const valU32: u32 = 0;
    const valU64: u64 = 0;

    const valF32: f32 = 0;
    const valF64: f64 = 0;
    const valF128: f128 = 0;
};
```

### Boolean

Booleans are assigned this way :
```
{
    const valBool: bool = true;
};
```

### String

Strings are assigned this way :
```
{
    const valStr: str = "Hello, World!\n";
};
```
All escaped characters are also available.

### Unary Operators

| Operator | Operand Type | Result Type | Example |
| - | - | - | - | - |
| `!` | `<any>` | `bool` | `!true` |

### Binary Operators


| Operator | LHS Type | RHS Type | Result Type | Example |
| - | - | - | - | - |
| `+` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | Biggest type  of `{LHS, RHS}` | `14.3 + 8.5` |
| `-` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | Biggest type  of `{LHS, RHS}` | `14.3 - 8.5` |
| `*` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | Biggest type  of `{LHS, RHS}` | `14.3 * 8.5` |
| `/` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | Biggest type  of `{LHS, RHS}` | `14.3 / 8.5` |
| `&&` | `bool` | `bool` | `bool` | `true && false` |
| `\|\|` | `bool` | `bool` | `bool` | `true \|\| false` |
| `<` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `bool` | `14.3 < 8.5` |
| `>` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `bool` | `14.3 > 8.5` |
| `<=` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `bool` | `14.3 <= 8.5` |
| `>=` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128` | `bool` | `14.3 >= 8.5` |
| `==` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128`, `bool`, `str` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128`, `bool`, `str` | `bool` | `14.3 == 8.5` |
| `!=` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128`, `bool`, `str` | `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `f128`, `bool`, `str` | `bool` | `14.3 != 8.5` |
| `&` | `bool` | Same as LHS | Same as LHS | `true & false` |
| `\|` | `bool` | Same as LHS | Same as LHS | `true \| false` |
| `^` | `bool` | Same as LHS | Same as LHS | `true ^ false` |

### Conditional Structures

> If-Else Structure

```
if(<condition>) {
    ...
} else if(<condition>) {
    ...
} else {
    ...
};
```

Note two important things :
1. The `;` at the end is required.
2. The else clause is required, because every statement has to have a value.

### Loops

> Loop

```
loop {
    ...
};
```

Loops infinitely (until `break` statement is met).

> While Loop

```
loop(<condition>) {
    ...
} or <value>;
```

Loops while `<condition>` is `true` (or until `break` statement is met).
If `<condition>` is instantly `false`, then the statement's value is `<value>`.

> For Loop

```
loop(<initial>, <condition>, <update>) {
    ...
} or <value>;
```

Before the first iteration, runs the `<initial>` statement.
Loops while `<condition>` is `true` (or until `break` statement is met), running `<update>` after each iteration.
If `<condition>` is instantly `false`, then the statement's value is `<value>`.

> Break

```
loop {
    break 4;
};
```

When breaking out of a loop, you need to provide a value.

### Variables

> Declaration

You can either declare a variable with a type :
```
<const/var> <name>: <type> = <value>;
```
Or infer the type :
```
<const/var> <name> := <value>;
```

> Assignation

```
<name> = <value>;
```
